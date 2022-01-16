



RaytracingAccelerationStructure gSceneAS : register(t0, space0);
RWTexture2D<float4> gOutput : register(u0, space0);

cbuffer CB : register(b0)
{
    float4x4 _ProjMatrix;
    float4x4 _ViewMatrix;
    float4x4 _InverseProjMatrix;
    float4x4 _InverseViewMatrix;
	float3 _EyePosition;
    float  _NearZ;
    float  _FarZ;
}


struct Payload
{
    float4 Color;
};

struct Built_in_attribute
{
    float BaryX;
    float BaryY;
};


[shader("raygeneration")]
void RayGeneration()
{

    uint2 launchIndex = DispatchRaysIndex().xy;
    uint2 dimensions = DispatchRaysDimensions().xy;

    float2 ndc = ((((float2)launchIndex + 0.5f) / (float)dimensions) * 2.f - 1.f);


    Payload payLoad;


    RayDesc ray;
    ray.Origin = _EyePosition;
    float4 target = mul(_InverseProjMatrix, float4(ndc.x, -ndc.y, 1, 1));
    ray.Direction = mul(_InverseViewMatrix, float4(target.xyz, 0));
    ray.TMin = _NearZ;
    ray.TMax = _FarZ;


    TraceRay(
        gSceneAS,
        RAY_FLAG_NONE,
        0xFF,
        0,
        0,
        0,
        ray,
        payload);

    gOutput[launchIndex] = payLoad.Color;

}
[shader("closesthit")]
void ClosestHit(inout Payload hit, Built_in_attribute attribute)
{
    payLoad = float4(1.0f, 1.0f, 1.0f, 1.0f);
}
[shader("miss")]
void Miss(inout Payload hit : SV_RayPayload)
{
    payLoad = float4(0.0f, 0.0f, 0.0f, 0.0f);
}