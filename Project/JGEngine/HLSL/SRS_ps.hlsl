#include"Common_ps.hlsli"
TEXTURE_START
TEXTURE_CONFIG(0, Default)
Texture2D Default_Texture;
TEXTURE_CONFIG(1, Normal)
Texture2D Normal_Texture;
TEXTURE_END

SAMPLERSTATE_CONFIG_START
SAMPLERSTATE_DEFAULT_WRAPMODE
SamplerState Default_WrapSampler;
SAMPLERSTATE_CONFIG_END


struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 worldPos : TEXCOORD1;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
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
	// 노멀 벡터 계산
	float4 normal = Normal_Texture.Sample(Default_WrapSampler, input.tex);
	float3 normalVec;
	normal = normalize((normal * 2) - 1.0f);
	normalVec = input.normal + normal.x * input.tangent + normal.y * input.binormal;
	normalVec = normalize(normalVec);
	normal = float4(normalVec, 1.0f);

    // 깊이 값 구하기
	float depthValue = input.worldPos.z / input.worldPos.w;


	output.worldPos = input.worldPos;
	output.albedo   = Default_Texture.Sample(Default_WrapSampler, input.tex);
	output.normal   = normal;
	output.depth    = float4(depthValue, depthValue, depthValue, 1.0f);

	// 위치값 
	return output;
}