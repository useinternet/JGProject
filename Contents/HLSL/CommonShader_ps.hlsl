#include"Common_ps.hlsli"

cbuffer Color
{
	float4 AmbientColor;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 worldPos : TEXCOORD1;
	float3 normal : NORMAL;
};
struct PixelOutput
{
	float4 worldPos : SV_Target0;
	float4 albedo   : SV_Target1;
	float4 normal   : SV_Target2;
	float4 depth    : SV_Target3;
};
PixelOutput main(PixelInputType input)
{
	PixelOutput output;

	// 깊이 값 구하기
	float depthValue = input.worldPos.z / input.worldPos.w;


	output.worldPos = input.worldPos;
	output.albedo = AmbientColor;
	output.normal = float4(input.normal,1.0f);
	output.depth = float4(depthValue, depthValue, depthValue, 1.0f);
	return output;
}