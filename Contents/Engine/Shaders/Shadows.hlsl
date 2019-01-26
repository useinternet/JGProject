//***************************************************************************************
// Shadows.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************
// Include common HLSL code.
#include "Common.hlsl"
#include"CommonInputLayout.hlsl"


struct VertexOut
{
	float4 PosH    : SV_POSITION;
};

VertexOut VS(VS_IN vin)
{
	VertexOut vout = (VertexOut)0.0f;
#ifdef SKINNED
    float weight[4] = {0.0f,0.0f,0.0f,0.0f};
    weight[0] = vin.BoneWeights.x;
    weight[1] = vin.BoneWeights.y;
    weight[2] = vin.BoneWeights.z;
    weight[3] = 1.0f - (weight[0] + weight[1] + weight[2]);
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; ++i)
    {
        posL += weight[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneID[i]]).xyz;
    }
   vin.PosL     = posL;
#endif

    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(posW, gViewProj);
    return vout;
}

void PS(VertexOut pin) 
{

}


