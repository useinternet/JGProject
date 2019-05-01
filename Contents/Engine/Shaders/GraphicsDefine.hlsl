
#include"GraphicsUtility.hlsli"


struct ObjectCB
{
    float4x4 World;
    uint     MaterialIndex;
    uint Padding[3];
};


#define DIFFUSE_TEXTURE_INDEX  0
#define NORMAL_TEXTURE_INDEX   1
#define SPECULAR_TEXTURE_INDEX 2

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





float3 CalcWorldPosition(float2 texC, uint depthTextureSlot)
{
    float z = gTexture[depthTextureSlot].Sample(gAnisotropicWrapSampler, texC).r;

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