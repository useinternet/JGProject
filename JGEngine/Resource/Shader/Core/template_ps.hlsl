

SamplerState gSampler  : register(s0);
Texture2D gTexture : register(t0, space0);


struct VS_Out
{
	float4 PosH  : SV_POSITION;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float2 texCoord : TEXCOORD;
};

float4 main(VS_Out pin) : SV_Target
{
    
    return gTexture.Sample(gSampler, pin.texCoord);
    //return float4(pin.texCoord,1,1);
}