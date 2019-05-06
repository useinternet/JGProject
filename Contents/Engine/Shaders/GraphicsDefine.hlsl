
#include"GraphicsUtility.hlsli"
#ifndef __GRAPHICSDEFINE_HLSL__
#define __GRAPHICSDEFINE_HLSL__





#define SCENE_ALBEDO 0
#define SCENE_NORMAL 1
#define SCENE_SPECULAR 2
#define SCENE_DEPTH 3

struct ObjectCB
{
    float4x4 World;
    uint MaterialIndex;
    uint Padding[3];
};

struct MaterialCB
{
    float3 SurfaceColor;
    float  Roughness;
    float  Metalic;
    float  BlankParam[3];
};



struct GBufferPack
{
    float4 Albedo : SV_TARGET0;
    float4 Normal   : SV_TARGET1;
    float4 Specular : SV_TARGET2;
    float Depth     : SV_TARGET3;
};

struct SceneData
{
    float3 Albedo;
    float3 Normal;
    float3 F0;
    float3 WorldPosition;
    float Depth;
    bool IsBackGround;
};




//
StructuredBuffer<ObjectCB>   gObjects : register(t0, space0);
StructuredBuffer<MaterialCB> gMaterials : register(t0, space1);
//
Texture2D   gTexture[8] : register(t0, space2);
TextureCube gCubeMap[4] : register(t0, space3);
//

//
cbuffer PassCB : register(b0)
{
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float3 gToEye;
    float  gFarZ;
    float  gNearZ;
    float3 padding;
};


cbuffer SkinnedCB : register(b1)
{
    float4x4 gBoneTransform[96];
};
SamplerState gPointWrapSampler : register(s0);
SamplerState gPointClampSampler : register(s1);
SamplerState gLinearWrapSampler : register(s2);
SamplerState gLinearClampSampler : register(s3);
SamplerState gAnisotropicWrapSampler : register(s4);
SamplerState gAnisotropicClampSampler : register(s5);

float3 CalcWorldPosition(float2 texC, float depth)
{
    float z = depth;

    float x = 2.0f * texC.x - 1.0f;
    float y = 1.0f - 2.0f * texC.y;
    float4 PosInProj = float4(x, y, z, 1.0f);
    float4 PosInWorld = mul(PosInProj, gInvViewProj);
    PosInWorld.xyz /= PosInWorld.w;
    return PosInWorld.xyz;
}
float ConvertLinearDepth(float depth)
{
    float proj_A = gFarZ / (gFarZ - gNearZ);
    float proj_B = (-gNearZ) / (gFarZ - gNearZ);
    return proj_B / (depth - proj_A);
    return gProj._34 / (depth + gProj._33);
}


SceneData GetSceneData(float2 uv)
{
    SceneData data;
    float4 albedo = gTexture[SCENE_ALBEDO].Sample(gLinearWrapSampler, uv);
    data.Albedo = albedo.rgb;
    if(albedo.a == 0.0f)
    {
        data.IsBackGround = true;
        return data;
    }
    data.IsBackGround = false;
    data.Depth = gTexture[SCENE_DEPTH].Sample(gLinearWrapSampler, uv).r;
    data.WorldPosition = CalcWorldPosition(uv, data.Depth);
    data.Depth = ConvertLinearDepth(data.Depth);


    data.Normal = gTexture[SCENE_NORMAL].Sample(gAnisotropicWrapSampler, uv).xyz;
    data.Normal = (data.Normal * 2.0f) - 1.0f;

    return data;
}




float3 CalcBumpNormal(float3 normalMap, float3 normalW, float3 tangentW)
{
    float3 normalT = 2.0f * normalMap - 1.0f;

    float3 N = normalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}

float3 FresnelSchlick(float NDotL, float3 F0)
{
    return F0 + (1 - F0) * pow(1 - NDotL, 5);
}
float3 GetHalfVector(float3 L, float3 V)
{
    return normalize(L + V);
}

float DistributionGGX(float NDotH, float a)
{
    float a2 = a * a;
    float NdotH2 = NDotH * NDotH;
 
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = _PI * denom * denom;
  
    return nom / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float nom = NdotV;
    float denom = NdotV * (1.0 - roughness) + roughness;
	
    return nom / denom;
}
  
float GeometrySmith(float NDotV, float NDotL, float roughness)
{
    float ggx1 = GeometrySchlickGGX(NDotV, roughness);
    float ggx2 = GeometrySchlickGGX(NDotL, roughness);
	
    return ggx1 * ggx2;
}

float IBLGeomtrySchlickGGX(float NDotV, float roughnesss)
{
    float a = roughnesss * roughnesss;
    float k = (a * a) / 2;

    return GeometrySchlickGGX(NDotV, k);
}

float IBLGeometrySmith(float NDotV, float NDotL, float roughness)
{
    float ggx1 = IBLGeomtrySchlickGGX(NDotV, roughness);
    float ggx2 = IBLGeomtrySchlickGGX(NDotL, roughness);
	
    return ggx1 * ggx2;
}


float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
float2 Hammersley(uint i, uint N)
{
    return float2(float(i) / float(N), RadicalInverse_VdC(i));
}
float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
    float a = roughness * roughness;
	
    float phi = 2.0 * _PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangent = normalize(cross(up, N));
    float3 bitangent = cross(N, tangent);
	
    float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float2 IntegrateBRDF(float roughness, float NDotV)
{
    float3 V;
    V.x = sqrt(1.0f - NDotV * NDotV);
    V.y = 0.0f;
    V.z = NDotV;

    float A = 0; 
    float B = 0;
    float3 N = float3(0.0f, 0.0f, 1.0f);
    const int numSamples = 1024;
    for (int i = 0; i < numSamples; ++i)
    {
        float2 xi = Hammersley(i, numSamples);
        float3 H = ImportanceSampleGGX(xi, N, roughness);
        float3 L = 2 * dot(V, H) * H - V;

        float NDotL = saturate(L.z);
        float NDotH = saturate(H.z);
        float VDotH = saturate(dot(V, H));

        if(NDotL > 0)
        {
            float G = IBLGeometrySmith(NDotV, NDotL, roughness);
            float G_Vis = G * VDotH / (NDotH * NDotV);
            float Fc = pow(1 - VDotH, 5);
            A += (1 - Fc) * G_Vis;
            B += Fc * G_Vis;

        }
    }

    return float2(A, B) / numSamples;
}


float3 Specular(float NDotL, float NDotH, float NDotV, float3 F0, float roughness)
{
    float3 F = FresnelSchlick(NDotL, F0);
    float D = DistributionGGX(NDotH, roughness * roughness);
    float G = GeometrySmith(NDotV, NDotL, roughness);

    float3 numerator = F * D * G;
    float denominator = 4 * NDotV * NDotL;
    float3 specular = numerator / max(denominator, 0.00001f);

    return specular;
}
float3 Diffuse(float3 albedo)
{
    return albedo / _PI;
}










#endif