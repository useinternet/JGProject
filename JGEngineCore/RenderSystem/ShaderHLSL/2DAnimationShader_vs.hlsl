



cbuffer MatrixBuffer
{
	matrix WorldViewProjectionMatrix;
};
cbuffer AnimTransIncrementBuffer
{
	float WidthIncrement;
	float HeightIncrement;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	input.position.w = 1.0f;
	// 택스쳐 이동시키기.. 일단 출력..
	output.position = mul(input.position, WorldViewProjectionMatrix);

	// 텍스쳐 이동
	input.tex.x += WidthIncrement;
	input.tex.y += HeightIncrement;
	output.tex = input.tex;

	return output;
}