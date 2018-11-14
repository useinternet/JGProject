


Texture2D Default_Texture;
SamplerState Default_WrapSampler;


struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
};
float4 main(PixelInputType input) : SV_TARGET
{
	float4 color = Default_Texture.Sample(Default_WrapSampler,input.tex);
	return float4(1.0f,1.0f,1.0f,1.0f);
}