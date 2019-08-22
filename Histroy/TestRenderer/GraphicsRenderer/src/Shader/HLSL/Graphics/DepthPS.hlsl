#include"GBufferVS.hlsl"
#include"GraphicsFunctions.hlsl"



float PS(GBufferPS pin) : SV_TARGET
{
    float depth = pin.Depth;
    //return ConvertLinearDepth(depth, gPassCB.FarZ, gPassCB.NearZ);
    return depth;
}