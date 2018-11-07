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

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};
float4 main(PixelInputType input) : SV_TARGET
{
	float4 worldPos = T_WorldPos.Sample(ClampSampler, input.tex);
	float4 albedo   = T_Albedo.Sample(ClampSampler,input.tex);
	float4 normal   = T_Normal.Sample(ClampSampler, input.tex);
	float4 depth    = T_Depth.Sample(ClampSampler, input.tex);



	// À§Ä¡°ª 
	return depth;
}