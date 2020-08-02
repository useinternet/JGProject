


#define _PI  3.14159265359
#define _2PI 6.28318530718

struct SDObject
{
	float4x4 world;
};

cbuffer CBCamera : register(b0, space0)
{
	float4x4 g_view;
	float4x4 g_proj;
	float4x4 g_viewProj;
	float4x4 g_invView;
	float4x4 g_invProj;
	float4x4 g_invViewProj;


	float g_lensWidth;
	float g_lensHeight;
	float g_farZ;
	float g_nearZ;
};

cbuffer CBMaterialData : register(b1, space0)
{
#if defined(__DEFINE_MATERIAL_DATA__)
	__DEFINE_MATERIAL_DATA__
#endif
}

cbuffer CBAnimData : register(b2, space0)
{
	float4x4 animTransform[128];
};



StructuredBuffer<SDObject>  gObjectInstances : register(t0, space0);
Texture2D gInputTexture[8]   : register(t0, space1);
Texture2D gBindTexture[1000] : register(t0, space2);




SamplerState g_AnisotropicSampler : register(s0, space0);
SamplerState g_PointSampler       : register(s1, space0);
SamplerState g_LinearSampler      : register(s2, space0);