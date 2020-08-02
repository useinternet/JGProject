
struct VS_Out
{
	float4 posH     : SV_POSITION;
    float3 posW     : POSITION;
    float3 normalW  : NORMAL;
    float3 tangentW : TANGENT;
    float2 texCoord : TEXCOORD;
};

float4 ps(VS_Out pin) : SV_Target
{
    return float4(1.0f,1.0f,1.0f,1.0f);
}