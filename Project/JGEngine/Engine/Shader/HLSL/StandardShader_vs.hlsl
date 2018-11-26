
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix wvpMatrix;
};
struct VertexInputType
{
	float3 position : POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 worldPos  : TEXCOORD1;
	float4 Pos : TEXCOORD2;
	float3 normal : NORMAL;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;

};
PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	float4 position = float4(input.position, 1.0f);


	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(position, wvpMatrix);
	output.Pos = output.position;
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	output.worldPos = mul(position, worldMatrix);

	//// 노말 벡터 는 월드 매트릭스만 계산
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	return output;
}