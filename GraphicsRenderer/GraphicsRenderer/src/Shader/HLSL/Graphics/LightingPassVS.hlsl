



#ifndef __QUAD_HLSLI__
#define __QUAD_HLSLI__

static const float2 gTexCoords[6] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};




struct QuadPS
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD0;
};


QuadPS VS(uint vid : SV_VertexID)
{
    QuadPS vout;

    vout.TexC = gTexCoords[vid];
    vout.PosH = float4(2.0f * vout.TexC.x - 1.0f, 1.0f - 2.0f * vout.TexC.y, 0.0f, 1.0f);
    return vout;
}















#endif
