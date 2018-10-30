#include"Common_vs.hlsli"


cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

INPUTLAYOUT  // ������ �Դϴ�
// ������ �Դϴ�.
struct VertexInputType
{
	INPUTSLOT(0) /*
         �������Դϴ�.
	*/ float3 position : POSITION; // ������ �Դϴ�.
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};
PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	float4 position = float4(input.position, 1.0f);


	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// �븻 ���� �� ���� ��Ʈ������ ���
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);


	return output;
}