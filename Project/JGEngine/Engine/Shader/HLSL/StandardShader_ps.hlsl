#include"Common.hlsli"


Texture2D Default_Texture;
Texture2D Normal_Texture;
SamplerState Default_WrapSampler;

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
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
};
PS_TARGET_OUT main(PixelInputType input)
{
	float4 textureColor = Default_Texture.Sample(Default_WrapSampler, input.tex);
	float4 normalMap    = Normal_Texture.Sample(Default_WrapSampler, input.tex);
	float3 normalVec;
	//// 노멀 맵의 법선 정보를 받아온다.
	normalMap = normalize((normalMap * 2) - 1.0f);

	// 노멀 맵의 법선 정보를 토대로 법선벡터 생성
	normalVec = input.normal + normalMap.x * input.tangent + normalMap.y * input.binormal;
	normalVec = normalize(normalVec);

	return PackData(textureColor.rgb, normalVec, SpecularIntensity, SpecularPower);
}
