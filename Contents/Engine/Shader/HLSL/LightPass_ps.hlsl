

Texture2D T_Pos_Depth      : register(t0);
Texture2D T_Normal_SpecPow : register(t1);
Texture2D T_Albedo         : register(t2);
Texture2D T_Specular       : register(t3);

SamplerState ClampSampler;


struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 CameraDir : TEXCOORD1;
};
float4 main(PixelInputType input) : SV_TARGET
{
	float4 Pos_Depth      = T_Pos_Depth.Sample(ClampSampler, input.tex);
	float4 Normal_SpecPow = T_Normal_SpecPow.Sample(ClampSampler, input.tex);
	// °ª ÃßÃâ
	float3 Albedo        = T_Albedo.Sample(ClampSampler, input.tex).xyz;
	float3 SpecularColor = T_Specular.Sample(ClampSampler, input.tex).xyz;
	float3 WorldPos      = Pos_Depth.xyz;
	float  Depth         = Pos_Depth.w;
	float3 Normal        = Normal_SpecPow.xyz;
	float  SpecPow       = Normal_SpecPow.w;

	return float4(input.CameraDir, 1.0f);
}