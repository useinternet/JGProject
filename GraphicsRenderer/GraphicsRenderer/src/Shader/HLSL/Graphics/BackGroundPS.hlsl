#include"QuadVS.hlsl"
#include"RS_GCommon.hlsl"
#include"GraphicsFunctions.hlsl"



float4 PS(QuadPS pin) : SV_TARGET
{
    float depth = gTexture[0].Sample(gLinearWrap, pin.TexC);
    if (depth < 1.0f)
    {
        discard;
    }

   // return float4(1.0f, 1.0f, 1.0f, 1.0f);
    float3 v = normalize(CalcWorldPosition(pin.TexC, depth));

    //return float4(1.0f, 1.0f, 1.0f, 1.0f);
   return gCubeTexture[0].SampleLevel(gLinearWrap, v, 0.0f);

}