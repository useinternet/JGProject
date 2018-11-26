
Texture2D T_Pos_Depth : register(t0);
SamplerState ClampSampler;

cbuffer PassBuffer
{
	matrix LightViewProjMatrix;
};
struct VertexInputType
{
	float3 position : POSITION;
	float2 tex      : TEXCOORD0;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 depthPos : TEXCOORD0;
	float  depthZ   : TEXCOORD1;
};
PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	float4 Pos_Depth = T_Pos_Depth.Sample(ClampSampler, input.tex).xyz;


	float4 position = float4(Pos_Depth.xyz, 1.0f);
	output.position = mul(position, LightViewProjMatrix);
	output.depthPos = output.position;
	output.depthZ = Pos_Depth.w;


	return output;
}