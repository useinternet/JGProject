//***************************************************************************************
// DrawNormals.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 0
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include common HLSL code.
#include "Common.hlsl"

struct VertexIn
{
	float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentU : TANGENT;
};

struct VertexOut
{
	float4 PosH     : SV_POSITION;
    float3 NormalW  : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

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


