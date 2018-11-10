#include"Common_ps.hlsli"


TEXTURE_START
Texture2D T_WorldPos : register(t0);
Texture2D T_Albedo : register(t1);
Texture2D T_Normal : register(t2);
Texture2D T_Depth : register(t3);
TEXTURE_END

SAMPLERSTATE_CONFIG_START
SAMPLERSTATE_DEFAULT_CLAMPMODE
SamplerState ClampSampler;
SAMPLERSTATE_CONFIG_END


cbuffer DirectionalLight
{
	float4 DL_AmbientDown : packoffset(c0);
	float4 DL_AmbientUp   : packoffset(c1);
	float4 DL_LightDir    : packoffset(c2);
	float4 DL_LightColor  : packoffset(c3);
	float4 DL_CameraPos   : packoffset(c4);
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};
float3 CalcDirectionLight(float3 albedo, float3 normal)
{
	float3 finalColor;
	float up = normal.y * 0.5 + 0.5;
	float3 Ambient = DL_AmbientDown.xyz + up * DL_AmbientUp.xyz;
	finalColor = Ambient * albedo;


	float3 LDir = -DL_LightDir.xyz;
	float NdotL = dot(LDir, normal);
	finalColor += saturate(DL_LightColor.rgb * NdotL);
	finalColor = saturate(finalColor) * albedo;
	return finalColor;
}
float4 main(PixelInputType input) : SV_TARGET
{
	float3 worldPos = T_WorldPos.Sample(ClampSampler, input.tex).xyz;
	float3 albedo   = T_Albedo.Sample(ClampSampler,input.tex).xyz;
	float3 normal   = T_Normal.Sample(ClampSampler, input.tex).xyz;
	float  depth    = T_Depth.Sample(ClampSampler, input.tex).x;

	normal.xyz = normalize(normal.xyz);

	return float4(CalcDirectionLight(albedo, normal),1.0f);
}