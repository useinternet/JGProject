

Texture2D gFinalTexture : register(t0);
SamplerState gsamAnisotropicWrap : register(s0);

static const float2 gTexCoords[6] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};
 
struct VsToPs
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD0;
};

VsToPs VS(uint vid : SV_VertexID)
{
    VsToPs vout;

    vout.TexC = gTexCoords[vid];
    vout.PosH = float4(2.0f * vout.TexC.x - 1.0f, 1.0f - 2.0f * vout.TexC.y, 0.0f, 1.0f);
 
    return vout;
}
float4 PS(VsToPs pin) : SV_Target
{
    return gFinalTexture.Sample(gsamAnisotropicWrap, pin.TexC);
}