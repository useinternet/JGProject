#include"GraphicsDefine.hlsl"
#include"QuadVS.hlsl"



float2 PS(VsToPs pin) : SV_Target
{
    float2 color = IntegrateBRDF(pin.TexC.y, pin.TexC.x);

    return color;
}