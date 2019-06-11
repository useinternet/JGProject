#include"SkyBoxVS.hlsl"




float4 PS(SkyBoxPS pin) : SV_TARGET
{

    float3 v = normalize(pin.PosL);

    //return float4(1.0f, 1.0f, 1.0f, 1.0f);
    return gCubeTexture[0].SampleLevel(gAnisotropicWrap, v, 0.0f);

}