
static const float2 arrPos[4] = {
	float2(-1.0f, 1.0f),
	float2( 1.0f, 1.0f),
	float2(-1.0f,-1.0f),
	float2( 1.0f,-1.0f),
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 cpPos	: TEXCOORD0;
};
PixelInputType main(uint VertexID : SV_VertexID)
{
	PixelInputType output;

	output.position = float4(arrPos[VertexID].xy, 0.0f, 1.0f);
	output.cpPos = output.position.xy;

	return output;
}