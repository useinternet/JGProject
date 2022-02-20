

Texture2D<uint>  _NormalDepthInput : register(t0, space0);
Texture2D<float4> _HitPositionInput : register(t1, space0);
StructuredBuffer<float4> _SampleSets : register(t2, space0);
RaytracingAccelerationStructure _SceneAS : register(t3, space0);

RWTexture2D<float>  _AOOutput : register(u0, space0);
RWTexture2D<float> _AORayDistanceOutput : register(u1, space0);

cbuffer CB : register(b0)
{
    uint _Seed;
    uint _NumSamplesPerSet;
    uint _NumSampleSets;
    uint _NumPixelsPerDimPerSet;

    uint2 _Resolution;
    float _SPP;  //1, 1, 1024, 1
    float _MaxAORayHitTime; // 22, 0.0f, 100.0f, 0.2f
    float _MinimumAmbientIllumination;   //0.07f, 0.0f, 1.0f, 0.01fju
};
struct AOPayload
{
    float tHit;
};
float3 GetRandomRayDirection(in uint2 srcRayIndex, in float3 surfaceNormal, in uint2 textureDim, in uint raySampleIndexOffset);
bool TraceAORayAndReportIfHit(out float tHit, in float3 rayOrigin, in float3 rayDir, in float TMax, in float3 normal);
float CalculateAO(out float tHit, in uint2 srcPixelIndex, in float3 rayOrigin, in float3 rayDir, in float3 normal);



[shader("raygeneration")]
void RayGeneration()
{
    uint2  launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (((launchIndex.xy + 0.5f) / dims.xy) * 2.f - 1.f);

    float3 normal;
    float  depth;

    DecodeNormalDepth( _NormalDepthInput[launchIndex], normal, depth);

    float ambientCoef = -1;
    float tHit = 0.0f;
    bool isValidHit = depth != 0;
    if(isValidHit)
    {
        float3 hitPosition = _HitPositionInput[launchIndex].xyz;
        ambientCoef = 0;
        for (uint r = 0; r < _SPP; r++)
        {
            float3 rayDirection = GetRandomRayDirection(launchIndex, normal, _Resolution, r);
            ambientCoef += CalculateAO(tHit, launchIndex, hitPosition, rayDirection, normal);
        }
    }
    _AOOutput[launchIndex] = ambientCoef;
    _AORayDistanceOutput[launchIndex] = tHit != 0 ? tHit : _MaxAORayHitTime;
}
[shader("closesthit")]
void ClosestHit(inout AOPayload payload, BuiltInTriangleIntersectionAttributes attribute)
{
    payload.tHit = RayTCurrent();
}
[shader("miss")]
void Miss(inout AOPayload payload : SV_RayPayload)
{
    payload.tHit = 0;
}




float hash(float3 p)
{
    p = frac(p*0.3183099 + .1);
    p *= 17.0;
    return frac(p.x*p.y*p.z*(p.x + p.y + p.z));
}
namespace RNG
{
    // Create an initial random number for this thread
    uint SeedThread(uint seed)
    {
        // Thomas Wang hash 
        // Ref: http://www.burtleburtle.net/bob/hash/integer.html
        seed = (seed ^ 61) ^ (seed >> 16);
        seed *= 9;
        seed = seed ^ (seed >> 4);
        seed *= 0x27d4eb2d;
        seed = seed ^ (seed >> 15);
        return seed;
    }

    // Generate a random 32-bit integer
    uint Random(inout uint state)
    {
        // Xorshift algorithm from George Marsaglia's paper.
        state ^= (state << 13);
        state ^= (state >> 17);
        state ^= (state << 5);
        return state;
    }

    // Generate a random float in the range [0.0f, 1.0f)
    float Random01(inout uint state)
    {
        return asfloat(0x3f800000 | Random(state) >> 9) - 1.0;
    }

    // Generate a random float in the range [0.0f, 1.0f]
    float Random01inclusive(inout uint state)
    {
        return Random(state) / float(0xffffffff);
    }


    // Generate a random integer in the range [lower, upper]
    uint Random(inout uint state, uint lower, uint upper)
    {
        return lower + uint(float(upper - lower + 1) * Random01(state));
    }
}
float3 GetRandomRayDirection(in uint2 srcRayIndex, in float3 normal, in uint2 textureDim, in uint raySampleIndexOffset)
{
    // Calculate coordinate system for the hemisphere.
    float3 u, v, w;
    w = normal;

    // Get a vector that's not parallel to w.
    float3 right = 0.3f * w + float3(-0.72f, 0.56f, -0.34f);
    v = normalize(cross(w, right));
    u = cross(v, w);

    // Calculate offsets to the pregenerated sample set.
    uint sampleSetJump;     // Offset to the start of the sample set
    uint sampleJump;        // Offset to the first sample for this pixel within a sample set.
    {
        // Neighboring samples NxN share a sample set, but use different samples within a set.
        // Sharing a sample set lets the pixels in the group get a better coverage of the hemisphere 
        // than if each pixel used a separate sample set with less samples pregenerated per set.

        // Get a common sample set ID and seed shared across neighboring pixels.
        uint numSampleSetsInX = (textureDim.x + _NumPixelsPerDimPerSet - 1) / _NumPixelsPerDimPerSet;
        uint2 sampleSetId = srcRayIndex / _NumPixelsPerDimPerSet;

        // Get a common hitPosition to adjust the sampleSeed by. 
        // This breaks noise correlation on camera movement which otherwise results 
        // in noise pattern swimming across the screen on camera movement.
        uint2  pixelZeroId = sampleSetId * _NumPixelsPerDimPerSet;
        float3 pixelZeroHitPosition = _HitPositionInput[pixelZeroId].xyz; 
        uint sampleSetSeed = (sampleSetId.y * numSampleSetsInX + sampleSetId.x) * hash(pixelZeroHitPosition) + _Seed;
        uint RNGState = RNG::SeedThread(sampleSetSeed);

        sampleSetJump = RNG::Random(RNGState, 0, _NumSampleSets - 1) * _NumSamplesPerSet;

        // Get a pixel ID within the shared set across neighboring pixels.
        uint2 pixeIDPerSet2D = srcRayIndex % _NumPixelsPerDimPerSet;
        uint pixeIDPerSet = pixeIDPerSet2D.y * _NumPixelsPerDimPerSet + pixeIDPerSet2D.x;

        // Randomize starting sample position within a sample set per neighbor group 
        // to break group to group correlation resulting in square alias.
        uint numPixelsPerSet = _NumPixelsPerDimPerSet * _NumPixelsPerDimPerSet;
        sampleJump = pixeIDPerSet + RNG::Random(RNGState, 0, numPixelsPerSet - 1) + raySampleIndexOffset;
    }

    // Load a pregenerated random sample from the sample set.
    float3 sample = _SampleSets[sampleSetJump + (sampleJump % _NumSamplesPerSet)].xyz;
    float3 rayDirection = normalize(sample.x * u + sample.y * v + sample.z * w);

    return rayDirection;
}

bool TraceAORayAndReportIfHit(out float tHit, in float3 rayOrigin, in float3 rayDir, in float TMax, in float3 normal)
{
    RayDesc rayDesc;
    rayDesc.Origin    = rayOrigin + 0.001 * normal;
    rayDesc.Direction = rayDir;
    rayDesc.TMin = 0.0;
	rayDesc.TMax = TMax;

    AOPayload payload = { TMax };

    uint rayFlags =
        // Ignore transparent surfaces for occlusion testing.
        RAY_FLAG_CULL_NON_OPAQUE;        

    TraceRay(_SceneAS,
        rayFlags,
        0xFF,
        0,
        0,
        0,
        rayDesc, payload);
    
    tHit = payload.tHit;

    return tHit != 0;
}

float CalculateAO(out float tHit, in uint2 srcPixelIndex, in float3 rayOrigin, in float3 rayDir, in float3 normal)
{
    float ambientCoef = 1;
    const float tMax = _MaxAORayHitTime;
    if (TraceAORayAndReportIfHit(tHit, rayOrigin, rayDir , tMax, normal))
    {
        float occlusionCoef = 1;
        // if (cb.applyExponentialFalloff)
        // {
        //     float theoreticalTMax = cb.maxTheoreticalAORayHitTime;
        //     float t = tHit / theoreticalTMax;
        //     float lambda = cb.exponentialFalloffDecayConstant;
        //     occlusionCoef = exp(-lambda * t * t);
        // }
        ambientCoef = 1 - (1 - _MinimumAmbientIllumination) * occlusionCoef;

        // Approximate interreflections of light from blocking surfaces which are generally not completely dark and tend to have similar radiance.
        // Ref: Ch 11.3.3 Accounting for Interreflections, Real-Time Rendering (4th edition).
        // The approximation assumes:
        //      o All surfaces' incoming and outgoing radiance is the same 
        //      o Current surface color is the same as that of the occluders
        // Since this sample uses scalar ambient coefficient, it usse the scalar luminance of the surface color.
        // This will generally brighten the AO making it closer to the result of full Global Illumination, including interreflections.
        // if (cb.approximateInterreflections)
        // {
        //     float3 surfaceAlbedo = g_texAOSurfaceAlbedo[srcPixelIndex].xyz;

        //     float kA = ambientCoef;
        //     float rho = cb.diffuseReflectanceScale * RGBtoLuminance(surfaceAlbedo);

        //     ambientCoef = kA / (1 - rho * (1 - kA));
        // }
    }

    return ambientCoef;
}