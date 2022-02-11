
//#include "Common.hlsli"

// Direct Lighting   -> Lighting / 
// Indirect Lighting -> Denosing
// Shadow            -> Denosing
// A0                -> Denosing
#define HitDistanceOnMiss 0

struct CB
{
    float4x4 InvViewProj;
    float4x4 PrevFrameViewProj;
    float4x4 PrevFrameInvViewProj;

    float3 EyePosition;
    float  NearZ;
    float3 PrevFrameEyePosition;
    float  FarZ;
};

struct TracerPayload
{
    uint  rayDepth;
    float tHit;
    float3 Direct;
    float3 Indirect;
    float3 AoDiffuse;
    float3 mv_VirtualHitPosition;
	float2 mv_EncodeNormal; // 이전 프레임 표면 노멀 법선
}


[shader("raygeneration")]
void RayGeneration()
{
    uint2  launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (((launchIndex.xy + 0.5f) / dims.xy) * 2.f - 1.f);

    RayDesc ray;
    ray.Origin = _EyePosition;
    float4 target = mul(float4(d.x, -d.y, 1, 1), _InvProjMatrix);
    ray.Direction = mul(float4(target.xyz, 0), _InvViewMatrix);
    ray.TMin = 0;
    ray.TMax = _FarZ;

    TracerPayload payload;
    // Shoot Ray

    bool hasCameraRayHitGeometry = payload.tHit != HitDistanceOnMiss;

    float rayLength = HitDistanceOnMiss;
    if(hasCameraRayHitGeometry)
    {
        rayLength = rayPayload.tHit;

        // Calc MotionVector
        /*
            // Variables prefixed with underscore _ denote values in the previous frame.
    float3 _hitViewPosition = _hitPosition - g_cb.prevFrameCameraPosition;
    float3 _cameraDirection = GenerateForwardCameraRayDirection(g_cb.prevFrameProjToViewCameraAtOrigin);
    _depth = dot(_hitViewPosition, _cameraDirection);

    // Calculate screen space position of the hit in the previous frame.
    float4 _clipSpacePosition = mul(float4(_hitPosition, 1), g_cb.prevFrameViewProj);
    float2 _texturePosition = ClipSpaceToTexturePosition(_clipSpacePosition);

    float2 xy = DispatchRaysIndex().xy + 0.5f;   // Center in the middle of the pixel.
    float2 texturePosition = xy / DispatchRaysDimensions().xy;

    return texturePosition - _texturePosition;
        */
        // Save MotionVector
        // Save ReprojectedNormalDepth ( mv_VirtualHitPosition, mv_EncodeNormal)  


        // Calc Linear Z-Depth
        // float3 CameraDir GenerateForwardCameraRayDirection
        // float linearDepth = rayLength * dot(ray.direction, cameraDirection);

        // Save Normal, LinearDpeth
        //g_rtGBufferNormalDepth[DTid] = EncodeNormalDepth(DecodeNormal(rayPayload.AOGBuffer.encodedNormal), linearDepth);
        // GBufferDepth = linearDepth

        // Save Diffuse
        // AoSurfaceAlbedo = AoDiffuse
    }
    else
    {
        //g_rtGBufferNormalDepth[DTid] = 0;
        //g_rtGBufferDepth[DTid] = 0;
        //g_rtAOSurfaceAlbedo[DTid] = 0;

        // Invalidate the motion vector - set it to move well out of texture bounds.
        //g_rtTextureSpaceMotionVector[DTid] = 1e3f;
        //g_rtReprojectedNormalDepth[DTid] = 0;
    } 

}


[shader("closesthit")]
void ClosestHit(inout TracerPayload payload, BuiltInTriangleIntersectionAttributes attribute)
{
        rayPayload.AOGBuffer.tHit = RayTCurrent();
    rayPayload.AOGBuffer.hitPosition = hitPosition;
    rayPayload.AOGBuffer.encodedNormal = EncodeNormal(normal);

    // Calculate hit position and normal for the current hit in the previous frame.
    // Note: This is redundant if the AOGBuffer gets overwritten in the Shade function. 
    // However, delaying this computation to post-Shade which casts additional rays results 
    // in bigger live state carried across trace calls and thus higher overhead.
    {

        /*
    float3 _hitObjectPosition;
    if (l_materialCB.isVertexAnimated)
    {
        float3 _vertices[3] = {
            l_verticesPrevFrame[vertexIndices[0]].position,
            l_verticesPrevFrame[vertexIndices[1]].position,
            l_verticesPrevFrame[vertexIndices[2]].position };
        _hitObjectPosition = HitAttribute(_vertices, attr);
    }
    else // non-vertex animated geometry 
    {
        _hitObjectPosition = hitObjectPosition;
    }

    // Transform the hit object position to world space.
    _BLASTransform = g_prevFrameBottomLevelASInstanceTransform[BLASInstanceIndex];
    return mul(_BLASTransform, float4(_hitObjectPosition, 1));
        */
        float3x4 _BLASTransform;
        rayPayload.AOGBuffer._virtualHitPosition = GetWorldHitPositionInPreviousFrame(HitObjectPosition(), InstanceIndex(), indices, attr, _BLASTransform);

        // Calculate normal at the hit in the previous frame.
        // BLAS Transforms in this sample are uniformly scaled so it's OK to directly apply the BLAS transform.
        rayPayload.AOGBuffer._encodedNormal = EncodeNormal(normalize(mul((float3x3)_BLASTransform, objectNormal)));
    }
}
// Hit Shader
