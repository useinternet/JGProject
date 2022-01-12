RWTexture2D<float4> gColorOutput : register(u0);
RaytracingAccelerationStructure gSceneAS : register(t0);

struct Payload
{
	float4 Color;
};


[shader("raygeneration")]
void RayGeneration() : SV_TARGET
{
	uint2 launchIndex = DispatchRaysIndex().xy;
	uint2 dimensions = DispatchRaysDimensions().xy;

	float2 ndc = ((((float2)launchIndex + 0.5f) / (float)dimensions) *2.f - 1.f);

	RayDesc ray;
	ray.Origin = float3(ndc.x, -ndc.y, -1.0f);
	ray.Direction = float3(0.0f,0.0f,1.0f);
	ray.TMin = 0.1f;
	ray.TMax = 1000.0f;

	Payload payload;
	TraceRay(
		gSceneAS,
		RAY_FLAG_NONE,
		0xFF,
		0,
		0,
		0,
		ray,
		payload
	);


	gColorOutput[launchIndex] = payload.Color;
}

