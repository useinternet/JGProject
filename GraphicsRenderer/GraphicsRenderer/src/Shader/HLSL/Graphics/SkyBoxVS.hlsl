#include"RS_GCommon.hlsl"


struct SkyBoxVS
{
    float3 PosL : POSITION;
};

struct SkyBoxPS
{
     float4 PosH : SV_POSITION;
     float3 PosL : POSITION;
};




SkyBoxPS VS(SkyBoxVS vin)
{
    SkyBoxPS vout;

    ObjectCB obj = gObjectCBs[0];

    vout.PosH = mul(float4(vin.PosL, 1.0f), obj.World);
    vout.PosH = mul(vout.PosH, gPassCB.ViewProj);

    vout.PosL = vin.PosL;



    return vout;
}




