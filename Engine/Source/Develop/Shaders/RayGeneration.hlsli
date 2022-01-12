




RWTexture2D<float4> gColorOutput : register(u0);

[shader("raygeneration")]
void RayGeneration()
{
	uint2 launchIndex = DispatchRaysIndex().xy;
	uint2 dimensions = DispatchRaysDimensions().xy;
	gColorOutput[launchIndex] = float4(0.4f, 0.2f, 0.0f, 1.0f);
}