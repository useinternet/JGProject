


#define _PI  3.14159265359
#define _2PI 6.28318530718

// ShadowFilter
// 
#define SHADOWFILTER_PCF    0
#define SHADOWFILTER_PCF3X3 1



struct SDObject
{
	float4x4 world;
};
struct SDAmbientLight
{

};
struct SDDirectionalLight
{
	float4x4 viewProj;
	float4 diffuse;
	float3 dir;
	float padding;
	float4  depthBias;
	float4 cascadeOffsetX;
	float4 cascadeOffsetY;
	float4 cascadeScale;
};

struct SDPointLight
{
	float4x4 proj;
	float4x4 viewProj[6];
	float4 diffuse;

	float3 position;
	float att0;

	float att1;
	float att2;
	float inRange;
	float outRange;

	float  depthBias;
};
struct SDSpotLight
{
	float4x4 viewProj;
	float4 diffuse;

	float3 position;
	float  att0;

	float att1;
	float att2;
	float inCone;
	float outCone;

	float3 dir;
	float  inRange;
	
	float  outRange;
	float  depthBias;


};
struct SDPassData
{
	int PointLightCount;
	int SpotLightCount;
	int DirectionalLightCount;
	int ShadowFilter;
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
	float3 g_EyePos;
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

cbuffer CBDirectionalLight : register(b3, space0)
{
	SDDirectionalLight gDirectionalLight;
};

cbuffer CBPassData : register(b4, space0)
{
	SDPassData gPassData;


};







StructuredBuffer<SDObject>      gObjectInstances : register(t0, space0);
StructuredBuffer<SDPointLight>  gPointLights     : register(t0, space1);
StructuredBuffer<SDSpotLight>   gSpotLights      : register(t0, space2);
Texture2D gInputTexture[8]   : register(t0, space3);
Texture2D gBindTexture[1000] : register(t0, space4);
Texture2D gSLShadowMap[1000] : register(t0, space5);
TextureCube gPLShadowMap[1000] : register(t0, space6);
Texture2DArray<float> gDLShadowMap : register(t8, space3);

SamplerState g_AnisotropicSampler : register(s0, space0);
SamplerState g_PointSampler       : register(s1, space0);
SamplerState g_LinearSampler      : register(s2, space0);
SamplerComparisonState g_ShadowSampler      : register(s3, space0);







float3 BumpMapping(float3 normalT, float3 normalW, float3 tangentW)
{
	normalT = (normalT * 2.0f) - 1.0f;

	float3 N = normalize(normalW);
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);
	float3 normal = mul(normalT, TBN);
	return normal;
}

float ConvertLinearDepth(float depth)
{
	return (2.0f * g_nearZ) / (g_farZ + g_nearZ - depth * (g_farZ - g_nearZ));
}

float3 CalcWorldPos(float2 csPos, float depth)
{
	float4 position;
	position = float4(2.0f * csPos.x - 1.0f, 1.0f - 2.0f * csPos.y, depth, 1);
	position = mul(position, g_invViewProj);
	position /= position.w;
	return position.xyz;
}

float3 CalcDirectionalLighting(SDDirectionalLight DL, float3 N)
{
	/*
	
	*/


	float3 DLDir     = -normalize(DL.dir);
	float4 DLDiffuse = saturate(DL.diffuse);


	float DLDotN = saturate(dot(DLDir, N));

	return DLDiffuse * DLDotN;
}



float3 CalcPointLighting(SDPointLight PL, float3 WorldPos, float3 N)
{
	float3 PLDir = normalize(PL.position - WorldPos);

	float PLDotN = max(dot(N, PLDir), 0.0f);
	float distance = length(PL.position - WorldPos);
	float epsilon = 1.0f;
	if (distance > PL.inRange)
	{
		float inoutRange = PL.outRange - PL.inRange;
		float delta = distance - PL.inRange;
		epsilon = (inoutRange - delta) / inoutRange;
		if (epsilon < 0.0f) return float3(0, 0, 0);
	}
	float att = 1.0f / (PL.att0 + PL.att1 * distance + PL.att2 * distance * distance);
	float3 diffuse = PL.diffuse.xyz;

	diffuse *= att;
	return diffuse * epsilon;

}

float3 CalcSpotLighting(SDSpotLight SL, float3 WorldPos, float3 N)
{
	float3 SLDir = -normalize(SL.dir);
	float  theta = dot(normalize(SL.position - WorldPos), SLDir);
	if (theta < SL.outCone) return float3(0, 0, 0);


	float  SLDotN = max(dot(N, SLDir), 0.0f);
	float distance = length(SL.position - WorldPos);


	float alpha = 1.0f;
	if (distance > SL.inRange)
	{
		float inoutRange = SL.outRange - SL.inRange;
		float delta = distance - SL.inRange;
		alpha = (inoutRange - delta) / inoutRange;
		if (alpha < 0.0f) return float3(0, 0, 0);
	}

	float epsilon = SL.inCone - SL.outCone;
	epsilon = clamp((theta - SL.outCone) / epsilon, 0.0f, 1.0f);

	float  att = 1.0f / (SL.att0 + SL.att1 * distance + SL.att2 * distance * distance);
	float3 diffuse = SL.diffuse.xyz;
	diffuse *= att;

	return diffuse * epsilon * alpha;

}

float CalcDirectionalShadowFactor(SDDirectionalLight DL, float3 WorldPos)
{
	float4 shadowPosH = mul(float4(WorldPos, 1.0), DL.viewProj);

	float4 posCascadeSpaceX = (DL.cascadeOffsetX + shadowPosH.xxxx) * DL.cascadeScale;
	float4 posCascadeSpaceY = (DL.cascadeOffsetY + shadowPosH.yyyy) * DL.cascadeScale;

	float4 inCascadeX = abs(posCascadeSpaceX) <= 1.0;
	float4 inCascadeY = abs(posCascadeSpaceY) <= 1.0;
	float4 inCascade = inCascadeX * inCascadeY;

	float4 bestCascadeMask = inCascade;
	bestCascadeMask.yzw = (1.0 - bestCascadeMask.x) * bestCascadeMask.yzw;
	bestCascadeMask.zw = (1.0 - bestCascadeMask.y) * bestCascadeMask.zw;
	bestCascadeMask.w = (1.0 - bestCascadeMask.z) * bestCascadeMask.w;
	float bestCascade = dot(bestCascadeMask, float4(0.0, 1.0, 2.0, 3.0));
	float depthBias = dot(bestCascadeMask, DL.depthBias);
	float3 UVD;
	UVD.x = dot(posCascadeSpaceX, bestCascadeMask);
	UVD.y = dot(posCascadeSpaceY, bestCascadeMask);
	UVD.z = shadowPosH.z - depthBias;

	UVD.xy = 0.5 * UVD.xy + 0.5;
	UVD.y = 1.0 - UVD.y;


	if (gPassData.ShadowFilter == SHADOWFILTER_PCF)
	{
		float shadow = gDLShadowMap.SampleCmpLevelZero(g_ShadowSampler, float3(UVD.xy, bestCascade), UVD.z);
		shadow = saturate(shadow + 1.0 - any(bestCascadeMask));
		return shadow;
	}
	else if(gPassData.ShadowFilter == SHADOWFILTER_PCF3X3)
	{
		uint width, height, elements, numberofLevel;
		gDLShadowMap.GetDimensions(0, width, height, elements, numberofLevel);


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
			float shadow = gDLShadowMap.SampleCmpLevelZero(g_ShadowSampler, float3(UVD.xy + offsets[i], bestCascade), UVD.z);
			shadow = saturate(shadow + 1.0 - any(bestCascadeMask));
			percentLit += shadow;
		}
		return saturate(percentLit / 9.0f);
	}
	else
	{

	}
	return 1.0f;
}
float3 CascadeDebug(SDDirectionalLight DL, float3 WorldPos)
{
	float4 shadowPosH = mul(float4(WorldPos, 1.0), DL.viewProj);

	float4 posCascadeSpaceX = (DL.cascadeOffsetX + shadowPosH.xxxx) * DL.cascadeScale;
	float4 posCascadeSpaceY = (DL.cascadeOffsetY + shadowPosH.yyyy) * DL.cascadeScale;

	float4 inCascadeX = abs(posCascadeSpaceX) <= 1.0;
	float4 inCascadeY = abs(posCascadeSpaceY) <= 1.0;
	float4 inCascade = inCascadeX * inCascadeY;

	float4 bestCascadeMask = inCascade;
	bestCascadeMask.yzw = (1.0 - bestCascadeMask.x) * bestCascadeMask.yzw;
	bestCascadeMask.zw = (1.0 - bestCascadeMask.y) * bestCascadeMask.zw;
	bestCascadeMask.w = (1.0 - bestCascadeMask.z) * bestCascadeMask.w;
	return 0.5 * bestCascadeMask;
}



float CalcSpotShadowFactor(SDSpotLight SL, int index, float3 WorldPos)
{

	float4 ShadowPosH = mul(float4(WorldPos, 1.0f), SL.viewProj);

	ShadowPosH.xyz /= ShadowPosH.w;
	ShadowPosH.xy = 0.5 * ShadowPosH.xy + 0.5;
	ShadowPosH.y = 1.0 - ShadowPosH.y;

	ShadowPosH.z -= SL.depthBias;

	if (gPassData.ShadowFilter == SHADOWFILTER_PCF)
	{
		return saturate(gSLShadowMap[index].SampleCmpLevelZero(g_ShadowSampler, ShadowPosH.xy, ShadowPosH.z));
	}
	else if (gPassData.ShadowFilter == SHADOWFILTER_PCF3X3)
	{
		uint width, height, numMips;
		gSLShadowMap[index].GetDimensions(0, width, height, numMips);


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
			percentLit += gSLShadowMap[index].SampleCmpLevelZero(g_ShadowSampler, ShadowPosH.xy + offsets[i], ShadowPosH.z).r;
		}
		return saturate(percentLit / 9.0f);
	}
	else return 1.0f;
}
float CalcPointShadowFactor(SDPointLight PL, int index, float3 worldPos)
{
	float3 ToPixel = worldPos - PL.position;
	float3 ToPixelAbs = abs(ToPixel);
	float x = PL.proj._33; float y = PL.proj._43;
	float Z = max(ToPixelAbs.x, max(ToPixelAbs.y, ToPixelAbs.z));
	float Depth = (x * Z + y) / Z;
	Depth -= PL.depthBias;
	if (gPassData.ShadowFilter == SHADOWFILTER_PCF)
	{
		return saturate(gPLShadowMap[index].SampleCmpLevelZero(g_ShadowSampler, ToPixel, Depth).x);
	}
	else if (gPassData.ShadowFilter == SHADOWFILTER_PCF3X3)
	{
		uint width, height, numMips;
		gSLShadowMap[index].GetDimensions(0, width, height, numMips);


		float dx = 2.5f;

		const float3 offsets[27] =
		{
			float3(-dx,  -dx,  -dx), float3(0.0f,  -dx,  -dx),  float3(dx,  -dx,  -dx),
			float3(-dx,  0.0f, -dx), float3(0.0f,  0.0f, -dx),  float3(dx,  0.0f, -dx),
			float3(-dx,  dx,   -dx), float3(0.0f,  dx,   -dx),  float3(dx,  dx,   -dx),

			float3(-dx,  -dx,  0.0f), float3(0.0f,  -dx,  0.0f),  float3(dx,  -dx,  0.0f),
			float3(-dx,  0.0f, 0.0f), float3(0.0f,  0.0f, 0.0f),  float3(dx,  0.0f, 0.0f),
			float3(-dx,  dx,   0.0f), float3(0.0f,  dx,   0.0f),  float3(dx,  dx,   0.0f),

			float3(-dx,  -dx,  dx), float3(0.0f,  -dx,  dx),  float3(dx,  -dx,  dx),
			float3(-dx,  0.0f, dx), float3(0.0f,  0.0f, dx),  float3(dx,  0.0f, dx),
			float3(-dx,  dx,   dx), float3(0.0f,  dx,   dx),  float3(dx,  dx,   dx)
		};


		float percentLit = 0.0f;
		[unroll]
		for (int i = 0; i < 27; ++i)
		{
			percentLit += gPLShadowMap[index].SampleCmpLevelZero(g_ShadowSampler, ToPixel + offsets[i], Depth).r;
		}
		return percentLit / 27.0f;

	}
	else return 1.0f;
}




