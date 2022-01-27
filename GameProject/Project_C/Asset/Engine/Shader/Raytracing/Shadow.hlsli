
#include "Common.hlsli"


RaytracingAccelerationStructure _SceneAS : register(t0, space1);
StructuredBuffer<PointLight> 	_PointLightList : register(t0, space2);


Texture2D _WorldPos : register(t0, space0);
RWTexture2D<float> _Output : register(u0, space0);



cbuffer CB : register(b0)
{
    int    _PointLightCount;
}


struct Payload
{
    float Factor;
};


float ShootRay_PL(PointLight pl, float3 worldPos)
{
    float  distance  = length(pl.Position - worldPos);
   	float3 direction = normalize(pl.Position - worldPos);

    Payload payload;
	payload.Factor = 0.0f;

    // PointLight 
	if(distance <= pl.Range)
    {
        RayDesc ray;
        ray.Origin    = worldPos;
        ray.Direction = direction; //normalize(getConeSample(randSeed,direction , 0.999));;
        ray.TMin = 0.01;
        ray.TMax = distance;
    	TraceRay(
        	_SceneAS,
        	RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_SKIP_CLOSEST_HIT_SHADER,
        	0xFF,
        	0, 
        	0,
        	0, 
        	ray,
        	payload);
    }
	else
	{
		return 1.0f;
	}

    return payload.Factor;
}


[shader("raygeneration")]
void RayGeneration()
{
    uint2  launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);


    int totalLightCount = _PointLightCount;

	float3 worldPos    = _WorldPos[launchIndex].xyz;
	float shadowFactor = 0.0f;

	for(int i = 0; i< totalLightCount; ++i)
	{
		PointLight pl = _PointLightList[i];
		shadowFactor += ShootRay_PL(pl, worldPos);
	}

	shadowFactor /= totalLightCount;

	_Output[launchIndex] = shadowFactor;
}
[shader("closesthit")]
void ClosestHit(inout Payload payload, BuiltInTriangleIntersectionAttributes attribute)
{
    payload.Factor = 0.0f;
}
[shader("miss")]
void Miss(inout Payload payload : SV_RayPayload)
{
    payload.Factor = 1.0f;
}


