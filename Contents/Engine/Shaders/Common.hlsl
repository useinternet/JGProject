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

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);


cbuffer cbPerObject : register (b0)
{
	float4x4 gWorld;
	float4x4 gTexTransform;
	uint gMaterialIndex;
	uint gCubeMapIndex;
	uint gObjPad[2];
}
cbuffer cbPerPass : register(b1)
{
	float4x4 gView;
	float4x4 gProj;
	float4x4 gViewProj;
	float4x4 gInvView;
	float4x4 gInvProj;
	float4x4 gInvViewProj;
	float4x4 gViewProjTex;
	float3 gEyePosW;
	uint gSSAOTextureIndex;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;
	float4 gAmbientLight;
	uint  gSkyBoxIndex;
	uint  gDirLightCount;
	uint  gPointLightCount;
	uint  gSpotLightCount;
}
struct MaterialData
{
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float  Roughness;
	float  Refractive;
	float3 CustomPad;
	float4x4 MatTransform;
	uint TextureIndex[8];
};
Texture2D gTexture[1]: register(t0, space0);
StructuredBuffer<MaterialData> gMaterialData : register(t0, space1);
StructuredBuffer<Light> gLightData : register(t1, space1);
TextureCube gCubeMap[1] : register(t2, space1);

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	float3 normalT = 2.0f*normalMapSample - 1.0f;
	float3 N = unitNormalW;
	float3 T = normalize(tangentW - dot(tangentW, N)*N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	float3 bumpedNormalW = mul(normalT, TBN);

	return bumpedNormalW;
}
float3 BoxCubeMapLookUp(float3 rayOrigin, float3 unitRayDir,
	float3 boxCenter, float3 boxExtents)
{
	float3 p = rayOrigin - boxCenter;

	float3 t1 = (-p + boxExtents) / unitRayDir;
	float3 t2 = (-p - boxExtents) / unitRayDir;

	float3 tmax = max(t1, t2);
	float t = min(min(tmax.x, tmax.y), tmax.z);

	return p + t * unitRayDir;
}
float CalcShadowFactor(float4 shadowPosH, uint Index)
{
	shadowPosH.xyz /= shadowPosH.w;
	// ±Ì¿Ã∞™ √ﬂ√‚
	float depth = shadowPosH.z;

	// ≈ÿºø ªÁ¿Ã¡Ó
	uint width, height, numMips;
	gTexture[Index].GetDimensions(0, width, height, numMips);

	float dx = 1.0f / (float)width;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += gTexture[Index].SampleCmpLevelZero(gsamShadow,
			shadowPosH.xy + offsets[i], depth).r;
	}

	return percentLit / 9.0f;
}
float CalcSpotShadowFactor(float4 ShadowPosH, uint Index)
{
	// Transform the position to shadow clip space
	float3 UVD = ShadowPosH.xyz / ShadowPosH.w;

	// Convert to shadow map UV values
	UVD.xy = 0.5 * UVD.xy + 0.5;
	UVD.y = 1.0f- UVD.y;

	// Compute the hardware PCF value
	return gTexture[Index].SampleCmpLevelZero(gsamShadow, UVD.xy, UVD.z).r;
}
float CalcPointShadowFactor(float3 PosW, Light l)
{
	float3 ToPixel = PosW - l.Position;
	float3 ToPixelAbs = abs(ToPixel);
	float x = l.ShadowTransform._33; float y = l.ShadowTransform._43;
	float Z = max(ToPixelAbs.x, max(ToPixelAbs.y, ToPixelAbs.z));
	float Depth = (x * Z + y) / Z;
	return gCubeMap[l.ShadowMapIndex].SampleCmpLevelZero(gsamShadow, ToPixel, Depth).r;
}
