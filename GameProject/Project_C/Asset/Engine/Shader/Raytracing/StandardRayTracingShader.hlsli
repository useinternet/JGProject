#include "Common.hlsli"

[shader("raygeneration")]
void RayGeneration()
{
    uint2  launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (((launchIndex.xy + 0.5f) / dims.xy) * 2.f - 1.f);

    Payload payload;
    payload.RandSeed = initRand(launchIndex.x + launchIndex.y * dims.x, _FrameCount, 16);


    RayDesc ray;
    ray.Origin = _EyePosition;
    float4 target = mul(float4(d.x, -d.y, 1, 1), _InverseProjMatrix);
    ray.Direction = mul(float4(target.xyz, 0), _InverseViewMatrix);
    ray.TMin = 0;
    ray.TMax = _FarZ;


    TraceRay(
        _SceneAS,
        RAY_FLAG_NONE,
        0xFF,
        0,
        0,
        0,
        ray,
        payload);
    float shadow = payload.Shadow;
    _ShadowOutput[launchIndex.xy] = shadow;

}
[shader("closesthit")]
void ClosestHit(inout Payload payload, BuiltInTriangleIntersectionAttributes attribute)
{
    payload.Shadow = ShootShadowRay(payload.RandSeed);
}
[shader("miss")]
void Miss(inout Payload payload : SV_RayPayload)
{
    payload.Shadow = 0.0f;
}

[shader("closesthit")]
void ShadowHit(inout ShadowPayload payload, BuiltInTriangleIntersectionAttributes attribute)
{
    payload.Factor = 0.0f;
}
[shader("miss")]
void ShadowMiss(inout ShadowPayload payload : SV_RayPayload)
{
    payload.Factor = 1.0f;
}

