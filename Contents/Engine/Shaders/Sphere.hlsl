#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#include "LightingUtil.hlsl"

// »ùÇÃ·¯
SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
Texture2D gTexture0 : register(t0);


cbuffer cbPerObject : register (b0)
{
	float4x4 gWorld;
	float4x4 gTexTransform;
}
cbuffer cbPerPass : register(b1)
{
	float4x4 gView;
	float4x4 gProj;
	float4x4 gViewProj;
	float4x4 gInvView;
	float4x4 gInvProj;
	float4x4 gInvViewProj;
	float3 gEyePosW;
	float  gCustomPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;
	float4 gAmbientLight;
	Light gLights[MaxLights];
}
cbuffer cbMaterial : register(b2)
{
	float4 gDiffuseAlbedo;
	float3 gFresnelR0;
	float  gRoughness;
	float4x4 gMatTransform;
}
struct VertexIn
{
	float3 PosL     : POSITION;
	float3 NormalL  : NORMAL;
	float3 TangentL : TANGENT;
	float2 TexC     : TEXCOORD0;
};

struct VertexOut
{
	float3 PosL    : POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosL = vin.PosL;

	return vout;
}

//struct PatchTess
//{
//	float EdgeTess[4]   : SV_TessFactor;
//	float InsideTess[2] : SV_InsideTessFactor;
//};
//
//PatchTess ConstantHS(InputPatch<VertexOut, 4> patch, uint patchID : SV_PrimitiveID)
//{
//	PatchTess pt;
//
//	float3 centerL = 0.25f*(patch[0].PosL + patch[1].PosL + patch[2].PosL + patch[3].PosL);
//	float3 centerW = mul(float4(centerL, 1.0f), gWorld).xyz;
//
//	float d = distance(centerW, gEyePosW);
//
//	// Tessellate the patch based on distance from the eye such that
//	// the tessellation is 0 if d >= d1 and 64 if d <= d0.  The interval
//	// [d0, d1] defines the range we tessellate in.
//
//	const float d0 = 20.0f;
//	const float d1 = 100.0f;
//	float tess = 64.0f * saturate((d1 - d) / (d1 - d0));
//
//	// Uniformly tessellate the patch.
//
//	pt.EdgeTess[0] = tess;
//	pt.EdgeTess[1] = tess;
//	pt.EdgeTess[2] = tess;
//	pt.EdgeTess[3] = tess;
//
//	pt.InsideTess[0] = tess;
//	pt.InsideTess[1] = tess;
//
//	return pt;
//}



struct PatchTess
{
	float EdgeTess[3]   : SV_TessFactor;
	float InsideTess : SV_InsideTessFactor;
};

PatchTess ConstantHS(InputPatch<VertexOut, 3> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess pt;

	float3 centerL = 0.25f*(patch[0].PosL + patch[1].PosL + patch[2].PosL);
	float3 centerW = mul(float4(centerL, 1.0f), gWorld).xyz;

	float d = distance(centerW, gEyePosW);

	// °Å¸®¿¡ µû¸¥ Å×¼¿

	const float d0 = 30.0f;
	const float d1 = 200.0f;
	float tess = 2.0f * saturate((d1 - d) / (d1 - d0));


	pt.EdgeTess[0] = tess;
	pt.EdgeTess[1] = tess;
	pt.EdgeTess[2] = tess;

	pt.InsideTess = tess;

	return pt;
}

struct HullOut
{
	float3 PosL : POSITION;
};

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(16.0f)]
HullOut HS(InputPatch<VertexOut, 3> p,
	uint i : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	HullOut hout;

	hout.PosL = p[i].PosL;

	return hout;
}

struct DomainOut
{
	float4 PosH : SV_POSITION;
};

// The domain shader is called for every vertex created by the tessellator.  
// It is like the vertex shader after tessellation.
[domain("tri")]
DomainOut DS(PatchTess patchTess,
	float3 uvw : SV_DomainLocation,
	const OutputPatch<HullOut, 3> quad)
{
	DomainOut dout;

	float3 p =
		uvw.x * quad[0].PosL + 
		uvw.y * quad[1].PosL +
		uvw.z * quad[2].PosL;

	float4 posW = mul(float4(p, 1.0f), gWorld);
	dout.PosH = mul(posW, gViewProj);

	return dout;
}

float4 PS(DomainOut pin) : SV_Target
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
