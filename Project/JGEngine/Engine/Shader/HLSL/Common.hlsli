
static const float2 g_SpecPowerRange = { 0.1f ,250.0f };

struct PS_TARGET_OUT
{
	float4 ColorSpecInt : SV_TARGET0;
	float4 Normal       : SV_TARGET1;
	float4 SpecPow      : SV_TARGET2;
};

PS_TARGET_OUT PackData(float3 Albedo, float3 Normal, float SpecIntensity, float SpecPower)
{
	PS_TARGET_OUT output;

	float SpecPowerNorm = (SpecPower - g_SpecPowerRange.x) / g_SpecPowerRange.y;

	output.ColorSpecInt = float4(Albedo.rgb, SpecIntensity);
	output.Normal       = float4(Normal.xyz * 0.5 + 0.5, 0.0f);
	output.SpecPow      = float4(SpecPowerNorm, 0.0f, 0.0f, 0.0f);

	return output;
}
