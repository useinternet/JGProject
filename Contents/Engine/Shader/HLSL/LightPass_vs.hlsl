

cbuffer PassBuffer
{
	matrix wvpMatrix;
	float3 CameraDir;
};


struct VertexInputType
{
	float3 position : POSITION;
	float2 tex      : TEXCOORD0;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 CameraDir : TEXCOORD1;
};
PixelInputType main(VertexInputType input)
{
	PixelInputType output;


	float4 position = float4(input.position, 1.0f);
	output.position = mul(position, wvpMatrix);
	output.tex = input.tex;
	output.CameraDir = CameraDir;

	return output;
}