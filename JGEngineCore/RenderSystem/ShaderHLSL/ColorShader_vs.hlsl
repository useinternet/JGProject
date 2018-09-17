



cbuffer MatrixBuffer
{
	matrix WorldViewProjectionMatrix;
};


struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR0;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	input.position.w = 1.0f;

	output.position = mul(input.position, WorldViewProjectionMatrix);
	output.color = input.color;


	return output;
}