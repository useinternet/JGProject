//***************************************************************************************
// DrawNormals.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

// Defaults for number of lights.

// Include common HLSL code.
#include "Common.hlsl"
#include"CommonInputLayout.hlsl"

struct VertexOut
{
	float4 PosH     : SV_POSITION;
    float3 NormalW  : NORMAL;
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
    float3 normalL = float3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; ++i)
    {
        posL += weight[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneID[i]]).xyz;
        normalL += weight[i] * mul(vin.NormalL, (float3x3)gBoneTransforms[vin.BoneID[i]]);
    }
   vin.PosL     = posL;
   vin.NormalL  = normalL;
#endif

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

    // Transform to homogeneous clip space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(posW, gViewProj);

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Write normal in view space coordinates
    float3 normalV = mul(pin.NormalW, (float3x3)gView);

	return float4(normalV, 0.0f);
}


