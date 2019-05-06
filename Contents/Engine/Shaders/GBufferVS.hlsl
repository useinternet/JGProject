#include"GraphicsDefine.hlsl"


#ifndef __GBUFFERVS_HLSL__
#define __GBUFFERVS_HLSL__

struct ToVs
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
struct VsToPs
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION0;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexC : TEXCOORD;
    float Depth : DEPTH;
    uint InstanceID : INSTANCE;
};
VsToPs VS(ToVs vin, uint instanceID : SV_InstanceID)
{
    VsToPs output;
    ObjectCB obj = gObjects[instanceID];
    float4x4 world = obj.World;
    output.InstanceID = instanceID;

#ifdef USE_SKINNED
    float3 posL = float3(0.0f,0.0f,0.0f);
    float3 normalL = float3(0.0f,0.0f,0.0f);
    float3 tangentL = float3(0.0f,0.0f,0.0f);

    for(int i =0; i<4; ++i)
    {
       posL += vin.BoneWeights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransform[vin.BoneID[i]]).xyz;
       normalL += vin.BoneWeights[i] * mul(vin.NormalL, (float3x3)gBoneTransform[vin.BoneID[i]]);
       tangentL += vin.BoneWeights[i] * mul(vin.TangentL, (float3x3)gBoneTransform[vin.BoneID[i]]);
    }
    vin.PosL     = posL;
    vin.NormalL  = normalL;
    vin.TangentL = tangentL;
#endif



    float4 PosW = mul(float4(vin.PosL, 1.0f), world);


    output.PosH = mul(PosW, gViewProj);
    output.PosW = PosW.xyz;
    output.NormalW = mul(vin.NormalL, (float3x3) world);
    output.TangentW = mul(vin.TangentL, (float3x3) world);
    output.TexC = vin.TexC;
    output.Depth = output.PosH.z / output.PosH.w;
    return output;
}

#endif