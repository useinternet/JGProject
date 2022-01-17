struct PointLight
{
    float3 Position;
    float  Range;
    float3 Color;
    float  AttRange;
    float  Intensity;
    float  Att0;
    float  Att1;
    float  Att2;
};




RWTexture2D<float4> gOutput : register(u0, space0);
RaytracingAccelerationStructure gSceneAS : register(t0, space0);
StructuredBuffer<PointLight> _PointLightList : register(t0, space1);


float3 HitWorldPosition()
{
    return WorldRayOrigin() + RayTCurrent() * WorldRayDirection();
}


cbuffer CB : register(b0)
{
    float4x4 _ProjMatrix;
    float4x4 _ViewMatrix;
    float4x4 _InverseProjMatrix;
    float4x4 _InverseViewMatrix;
	float3 _EyePosition;
    float  _NearZ;
    float  _FarZ;
    int    _PointLightCount;
}


struct Payload
{
    float4 Color;
};

struct ShadowPayload
{
    bool Hit;
};


[shader("raygeneration")]
void RayGeneration()
{
    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (((launchIndex.xy + 0.5f) / dims.xy) * 2.f - 1.f);

    Payload payload;


    RayDesc ray;
    ray.Origin = _EyePosition;
    float4 target = mul(float4(d.x, -d.y, 1, 1), _InverseProjMatrix);
    ray.Direction = mul(float4(target.xyz, 0), _InverseViewMatrix);
    ray.TMin = 0;
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

    gOutput[launchIndex.xy] = payload.Color;

}
[shader("closesthit")]
void ClosestHit(inout Payload payload, BuiltInTriangleIntersectionAttributes attribute)
{
    float3 worldPos = HitWorldPosition();


    for(int i = 0; i<_PointLightCount; ++i)
    {
        PointLight pl = _PointLightList[i];
        
        float distance  = length(pl.Position - worldPos);
        float direction = normalize(pl.Position - worldPos);

        if(distance <= pl.Range)
        {
            ShadowPayload shadowPayload;
            shadowPayload.Hit = true;
            RayDesc shadowRay;
            shadowRay.Origin = worldPos;
            shadowRay.Direction = direction;
            shadowRay.TMin = 0.01;
            shadowRay.TMax = _FarZ;
        }
    }

    payload.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
}
[shader("miss")]
void Miss(inout Payload payload : SV_RayPayload)
{
    payload.Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
}

[shader("miss")]
void ShadowMiss(inout ShadowPayload payload : SV_RayPayload)
{
    payload.Hit = false;
}

