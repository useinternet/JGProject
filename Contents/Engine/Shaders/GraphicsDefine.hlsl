
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

//
StructuredBuffer<ObjectCB>   gObjects : register(t0, space0);
StructuredBuffer<MaterialCB> gMaterials : register(t0, space1);
//
Texture2D   gTexture[8] : register(t0, space2);
TextureCube gCubeMap[4] : register(t0, space3);



cbuffer PassCB : register(b0)
{
    float4x4 gViewProj;
    float3   gToEye;
    float    gDeltaTime;
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



