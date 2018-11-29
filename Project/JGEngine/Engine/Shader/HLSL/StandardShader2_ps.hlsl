#include"Common.hlsli"


cbuffer Material
{
	float3 Ambient;           // 기본 1
	float  ReflectIntensity;  // 기본 0
	float3 SpecularColor;     // 기본 1
	float  SpecularPower;     // 기본 1
	float3 Emissive;          // 기본 1
	float  SpecularIntensity;         // 기본 0
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 worldPos  : TEXCOORD1;
	float3 normal : NORMAL;
};
PS_TARGET_OUT main(PixelInputType input)
{
	input.normal = normalize(input.normal);

	return PackData(Ambient, input.normal, SpecularIntensity, SpecularPower);
}