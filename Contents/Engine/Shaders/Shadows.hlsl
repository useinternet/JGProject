//***************************************************************************************
// Shadows.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

// Include common HLSL code.
#include "Common.hlsl"

struct VertexIn
{
	float3 PosL    : POSITION;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(posW, gViewProj);
    return vout;
}

void PS(VertexOut pin) 
{

}


