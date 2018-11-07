#include"Common_vs.hlsli"


cbuffer MatrixBuffer
{
	matrix wvpMatrix;
	matrix worldMatrix;
};

INPUTLAYOUT  
struct VertexInputType
{
	INPUTSLOT(0) 
	float3 position : POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 worldPos : TEXCOORD1;
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

	// 월드 좌표
	output.worldPos = mul(position, worldMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// 노말 벡터 는 월드 매트릭스만 계산
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}