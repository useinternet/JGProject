#include"RS_GCommon.hlsl"
#include"QuadVS.hlsl"
static const float gamma = 2.2;
static const float exposure = 1.0;
static const float pureWhite = 1.0;


float3 LinearToGamma(float3 c)
{
    return pow(c.rgb, 1.0f / gamma);
}
float4 PS(QuadPS pin) : SV_TARGET
{
    float3 color = gTexture[0].Sample(gLinearWrap, pin.TexC).rgb * exposure;

    float luminance = dot(color, float3(0.2126, 0.7152, 0.0722));
    float mappedLuminance = (luminance * (1.0 + luminance / (pureWhite * pureWhite))) / (1.0 + luminance);

    float3 mappedColor = (mappedLuminance / luminance) * color;
    return float4(LinearToGamma(mappedColor), 1.0);
}