


Texture2D Texture;
SamplerState WrapSamplerState;


cbuffer TextBuffer
{
	float4 TextColor;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
	float4 color;

    color = Texture.Sample(WrapSamplerState, input.tex);
	color.xyz = color.xyz * TextColor.xyz;
	color.a = TextColor.a;

	return color;
}