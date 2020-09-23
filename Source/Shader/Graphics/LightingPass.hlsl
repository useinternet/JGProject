#include "GraphicsCommon.hlsl"


#define LIGHT_INPUT_TEXTURE_ALBEDO 0
#define LIGHT_INPUT_TEXTURE_NORMAL 1
#define LIGHT_INPUT_TEXTURE_DEPTH  2
struct VS_OUT
{
	float4 posH : SV_POSITION;
	float2 tex  : TEXCOORD;
};


VS_OUT vs(uint vertexID : SV_VertexID)
{
	VS_OUT vout;
	vout.tex = float2((vertexID << 1) & 2, vertexID & 2);
	vout.posH = float4(vout.tex * float2(2, -2) + float2(-1, 1), 0, 1);
	return vout;
}

float4 ps(VS_OUT pin) : SV_Target
{
	float depth = gInputTexture[LIGHT_INPUT_TEXTURE_DEPTH].Sample(g_AnisotropicSampler, pin.tex).x;
	depth = saturate(depth);


	float4 albedo = gInputTexture[LIGHT_INPUT_TEXTURE_ALBEDO].Sample(g_AnisotropicSampler, pin.tex);
	float4 normal = gInputTexture[LIGHT_INPUT_TEXTURE_NORMAL].Sample(g_AnisotropicSampler, pin.tex);
	normal.xyz = (normal.xyz * 2.0f) - 1.0f;
	normal.xyz = normalize(normal.xyz);

	float4 Color     = float4(0.25f, 0.25f, 0.35f, 1.0f);
	float3 WorldPos = CalcWorldPos(pin.tex, depth);

	
	if (gPassData.DirectionalLightCount == 1)
	{
		float ShadowFactor = CalcDirectionalShadowFactor(gDirectionalLight, WorldPos);
		Color.xyz += (ShadowFactor * CalcDirectionalLighting(gDirectionalLight, normal.xyz));
	}
	// PointLight
	int PLCnt = gPassData.PointLightCount;
	for (int i = 0; i < PLCnt; ++i)
	{
		
		float ShadowFactor = CalcPointShadowFactor(gPointLights[i], i, WorldPos);
		Color.xyz += (ShadowFactor * CalcPointLighting(gPointLights[i], WorldPos, normal.xyz));
	}
	// SpotLight
	int SLCnt = gPassData.SpotLightCount;
	for (int i = 0; i < SLCnt; ++i)
	{
		float ShadowFactor = CalcSpotShadowFactor(gSpotLights[i], i, WorldPos);
		Color.xyz += (ShadowFactor * CalcSpotLighting(gSpotLights[i], WorldPos, normal.xyz));
	}

	return saturate(Color * albedo);
}