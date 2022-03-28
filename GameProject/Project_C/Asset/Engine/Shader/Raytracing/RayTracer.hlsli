
//#include "Common.hlsli"

// Direct Lighting   -> Lighting / 
// Indirect Lighting -> Denosing
// Shadow            -> Denosing
// A0                -> Denosing


[shader("raygeneration")]
void RayGeneration()
{
    uint2  launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (((launchIndex.xy + 0.5f) / dims.xy) * 2.f - 1.f);
    float4 target = mul(float4(d.x, -d.y, 1, 1), _InvProjMatrix);


    float3 rayOrigin = _EyePosition;
    float3 rayDir    = normalize(mul(float4(target.xyz, 0), _InvViewMatrix));
    float3 hitPosition;

    HaltonState hState;
	haltonInit(hState, launchIndex.x + 10, launchIndex.y + 10, 1, 1, _FrameCount, 1);
    uint indirctSeed = initRand(launchIndex.x + launchIndex.y * dims.x, _FrameCount, 16);
    uint shadowSeed  = initRand(launchIndex.x + launchIndex.y * dims.x, _FrameCount * 2, 16);

    // Direct Ray



    float3 directColor = ShootDirectRay(rayOrigin, rayDir, hitPosition);
    float  shadow      = ShootShadowRay(shadowSeed, hitPosition);



    float3 indirectColor = ShootIndirectRay(rayOrigin, rayDir, indirctSeed, hState, -1);


    _DirectOutput[launchIndex]        = float4(directColor, 1.0f);
    _IndirectRedOutput[launchIndex]   = indirectColor.x;
    _IndirectGreenOutput[launchIndex] = indirectColor.y;
    _IndirectBlueOutput[launchIndex]  = indirectColor.z;
    _ShadowOutput[launchIndex]        = saturate(shadow);
}


// Local //
cbuffer _DefaultCB__ : register(b0, space1)
{ }
StructuredBuffer<Vertex> Local_VertexBuffer   : register(t0, space1);
StructuredBuffer<Vertex> Prev_Local_VertexBuffer   : register(t1, space1);
StructuredBuffer<uint>   Local_IndexBuffer    : register(t2, space1);
Texture2D Null_Texture : register(t3, space1);

// Default Hit Shader
[shader("closesthit")]
void DirectClosestHit(inout DirectRayPayload payload, BuiltInTriangleIntersectionAttributes attribute)
{
    uint startIndex = PrimitiveIndex() * 3;
    const uint3 indices = { Local_IndexBuffer[startIndex], Local_IndexBuffer[startIndex + 1], Local_IndexBuffer[startIndex + 2] };
    Vertex vertices[3] = {
        Local_VertexBuffer[indices[0]],
        Local_VertexBuffer[indices[1]],
        Local_VertexBuffer[indices[2]] };

    Vertex v = HitAttribute(vertices, attribute);

    float4 textureColor = Null_Texture.SampleLevel(_PointWrap, v.Texcoord, 0);
    payload.Color = textureColor.xyz;
}
[shader("closesthit")]
void IndirectClosestHit(inout IndirectRayPayload payload, BuiltInTriangleIntersectionAttributes attribute)
{
    payload.Color = float3(0.0f,0.0f,0.0f);
}


// Default Miss Shader
[shader("miss")]
void DirectMiss(inout DirectRayPayload payload : SV_RayPayload)
{
    payload.Color = float3(0.0f,0.0f,0.0f);
}


[shader("miss")]
void IndirectMiss(inout IndirectRayPayload payload : SV_RayPayload)
{
    payload.Color = float3(0.0f, 0.0f, 0.0f);
}


