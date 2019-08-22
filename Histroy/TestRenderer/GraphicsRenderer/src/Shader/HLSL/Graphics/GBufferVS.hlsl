#include"GraphicsStructDefine.hlsl"
#include"RS_GCommon.hlsl"


#ifndef __GBUFFERVS_HLSL__
#define __GBUFFERVS_HLSL__

struct GBufferVS
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float2 TexC : TEXCOORD;
#ifdef USE_SKINNED
    float4 BoneWeights : WEIGHTS;
    uint4  BoneID      : BONEID;
#endif
};



struct GBufferPS
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION0;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexC : TEXCOORD;
    float Depth : DEPTH;
    uint InstanceID : INSTANCE;
};

GBufferPS VS(GBufferVS vin, uint instanceID : SV_InstanceID)
{
   
//#ifdef USE_SKINNED
//    float3 posL = float3(0.0f,0.0f,0.0f);
//    float3 normalL = float3(0.0f,0.0f,0.0f);
//    float3 tangentL = float3(0.0f,0.0f,0.0f);

//    for(int i =0; i<4; ++i)
//    {
//       posL += vin.BoneWeights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransform[vin.BoneID[i]]).xyz;
//       normalL += vin.BoneWeights[i] * mul(vin.NormalL, (float3x3)gBoneTransform[vin.BoneID[i]]);
//       tangentL += vin.BoneWeights[i] * mul(vin.TangentL, (float3x3)gBoneTransform[vin.BoneID[i]]);
//    }
//    vin.PosL     = posL;
//    vin.NormalL  = normalL;
//    vin.TangentL = tangentL;
//#endif

    GBufferPS pout;
    ObjectCB obj = gObjectCBs[instanceID];
    float4x4 world = obj.World;
    float4x4 viewProj = gPassCB.ViewProj;

    float4 posW = mul(float4(vin.PosL, 1.0f), world);
    pout.PosH = mul(posW, viewProj);
    pout.PosW = posW.xyz;
    pout.NormalW = mul(vin.NormalL, (float3x3)world);
    pout.NormalW = normalize(pout.NormalW);
    pout.TangentW = mul(vin.TangentL, (float3x3) world);
    pout.TangentW = normalize(pout.TangentW);
    pout.InstanceID = instanceID;
    pout.Depth = pout.PosH.z / pout.PosH.w;
    pout.TexC = vin.TexC;
    return pout;
}


#endif