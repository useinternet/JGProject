#include "GraphicsCommon.hlsl"

struct VS_IN
{
	float3 posL     : POSITION;
};
struct VS_OUT
{
	float4 posH : SV_POSITION;
	//float depth : DEPTH;
};



VS_OUT vs(VS_IN vin, uint insID : SV_InstanceID) 
{
	VS_OUT   vout;

	//  float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	float4  posW = mul(float4(vin.posL, 1.0f), gObjectInstances[insID].world);
	vout.posH = mul(posW, g_viewProj);
	//vout.depth = vout.posH.z / vout.posH.w;
	return vout;

}

void ps(VS_OUT pin) 
{
	//return pin.depth;
}