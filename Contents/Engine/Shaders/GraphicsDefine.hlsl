



struct ObjectCB
{
    float4x4 World;
};



struct MaterialCB
{
    float4 DiffuseAlbedo;
};



StructuredBuffer<ObjectCB> gObjects : register(t0, space0);
StructuredBuffer<MaterialCB> gMaterials : register(t0, space1);

cbuffer PassCB : register(b0)
{
    float4x4 gViewProj;
    float3 gToEye;
    float gDeltaTime;
};
cbuffer SkinnedBuffer : register(b1)
{
#ifdef USE_SKINNED
    float4x4 gBoneTransform[96];
#endif
}
Texture2D gDiffuseTexture : register(t0, space2);
TextureCube gCubeMap : register(t0, space3);

SamplerState gPointWrapSampler : register(s0);
SamplerState gPointClampSampler : register(s1);
SamplerState gLinearWrapSampler : register(s2);
SamplerState gLinearClampSampler : register(s3);
SamplerState gAnisotropicWrapSampler : register(s4);
SamplerState gAnisotropicClampSampler : register(s5);



