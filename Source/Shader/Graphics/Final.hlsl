
#include "GraphicsCommon.hlsl"

//struct VS_OUT
//{
//	float4 posH : SV_POSITION;
//	float2 tex  : TEXCOORD;
//};
//
//
//VS_OUT vs(uint vertexID : SV_VertexID)
//{
//	VS_OUT vout;
//	vout.tex = float2((vertexID << 1) & 2, vertexID & 2);
//	vout.posH = float4(vout.tex * float2(2, -2) + float2(-1, 1), 0, 1);
//	return vout;
//}
//
//float4 ps(VS_OUT pin) : SV_Target
//{
//	return gInputTexture[0].Sample(g_AnisotropicSampler, pin.tex);
//}


struct VS_IN
{

#if defined(__SDPAPEROBJECT__)
	float3 posL : POSITION;
	float3 tex  : TEXCOORD;
#endif

#if defined(__SDSTATICOBJECT__)

	float3 posL     : POSITION;
	float3 normalL  : NORMAL;
	float3 tangentL : TANGENT;
	float2 tex      : TEXCOORD;

#endif

#if defined(__SDSKINNEDOBJECT__)

	float3 posL     : POSITION;
	float3 normalL  : NORMAL;
	float3 tangentL : TANGENT;
	float2 tex      : TEXCOORD;
	float4 boneWeight : BONEWEIGHT;
	int4   bondID : BONEID;

#endif


};
struct VS_OUT
{
#if defined(__RENDERER_MODE_3D__)
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float2 tex   : TEXCOORD;
#endif

#if defined(__RENDERER_MODE_PAPER__)
	float4 posH : SV_POSITION;
	float2 tex   : TEXCOORD;
#endif

#if defined(__RENDERER_MODE_GUI__)
	float4 posH : SV_POSITION;
	float2 tex   : TEXCOORD;
#endif

	// TODO()
};

struct PS_OUT
{
#if defined(__RENDERER_MODE_3D__)
	float4 Albedo : SV_Target0;
#endif

#if defined(__RENDERER_MODE_PAPER__)
	float4 Albedo : SV_Target0;
#endif
#if defined(__RENDERER_MODE_GUI__)
	float4 Albedo : SV_Target0;
#endif

};



VS_OUT vs(VS_IN vin, uint insID : SV_InstanceID)
{
	VS_OUT vout;
	//  3DRenderer 
#if defined(__RENDERER_MODE_3D__)

	float4x4 world;
	float4 posW = float4(0, 0, 0, 1);
	float3 normalW = float3(0, 0, 0);
	float3 tangentW = float3(0, 0, 0);
#if defined(__SDSTATICOBJECT__)
	world = gObjectInstances[insID].world;
	posW = mul(float4(vin.posL, 1.0f), world);
	normalW = normalize(mul(vin.normalL, (float3x3)world));
	tangentW = normalize(mul(vin.tangentL, (float3x3)world));
	// TODO 
#endif
#if defined(__SDSKINNEDOBJECT__)
	world = gObjectInstances[insID].world;
	posW = mul(float4(vin.posL, 1.0f), world);
	normalW = normalize(mul(vin.normalL, (float3x3)world));
	tangentW = normalize(mul(vin.tangentL, (float3x3)world));
	// TODO 
#endif
	vout.posH = mul(posW, g_viewProj);
	vout.posW = posW.xyz;
	vout.normalW = normalW;
	vout.tangentW = tangentW;
	vout.tex = vin.tex;


#endif


	// Paper Renderer
#if defined(__RENDERER_MODE_PAPER__)

	float4x4 world = gObjectInstances[insID].world;
	float4 posW = mul(float4(vin.posL, 1.0f), world);
	vout.posH = mul(posW, g_viewProj);
	vout.tex = vin.tex;

#endif

	// GUI Renderer
#if defined(__RENDERER_MODE_GUI__)
	float4x4 world = gObjectInstances[insID].world;
	float4 posW = mul(float4(vin.posL, 1.0f), world);
	vout.posH = mul(posW, g_viewProj);
	vout.tex = vin.tex;
#endif


	return vout;
}

PS_OUT ps(VS_OUT pin)
{
	PS_OUT pout;
	// 3DRenderer
#if defined(__RENDERER_MODE_3D__)

	pout.Albedo = float4(1.0f, 1.0f, 1.0f, 1.0f);


#if defined(__DEFINE_MATERIAL_CODE__)
	__DEFINE_MATERIAL_CODE__
#endif
		// TODO


#endif





	// Paper Renderer
#if defined(__RENDERER_MODE_PAPER__)
		pout.Albedo = float4(1.0f, 1.0f, 1.0f, 1.0f);


#if defined(__DEFINE_MATERIAL_CODE__)
	__DEFINE_MATERIAL_CODE__
#endif
		// TODO

#endif





	// GUI Renderer

#if defined(__RENDERER_MODE_GUI__)

		pout.Albedo = float4(1.0f, 1.0f, 1.0f, 1.0f);


#if defined(__DEFINE_MATERIAL_CODE__)
	__DEFINE_MATERIAL_CODE__
#endif
		// TODO(ps process)


#endif

		return pout;
}