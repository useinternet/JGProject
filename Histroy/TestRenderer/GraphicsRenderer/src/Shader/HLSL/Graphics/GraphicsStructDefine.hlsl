
#ifndef __GRAPHCISSTRUCTDEFINE_HLSL__
#define __GRAPHCISSTRUCTDEFINE_HLSL__

// ---    define 정의   --- //
#define _PI  3.14159265359f
#define _2PI 6.28318530718f
#define _PIDIV2 1.57079632679f


// ---    struct 타입 정의   --- //
struct ObjectCB
{
    float4x4 World;
};
struct PassCB
{
    float4x4 ViewProj;
    float4x4 View;
    float4x4 Proj;
    float4x4 InvViewProj;
    float4x4 InvView;
    float4x4 InvProj;
    float3 EyePos;
    float  FarZ;
    float  NearZ;
    uint LightCount;
    float2 padding;
};
struct LightCB
{
    uint type;
    float3 direction;
    float3 color;
    float indensity;
    float bias;
    float3 position;
    float attConstant1;
    float attConstant2;
    float range;
    float padding;
    float inCos;
    float outCos;
    float2 padding2;
};
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_SPOT 1
#define LIGHT_TYPE_POINT 2
/* GBuffer 데이터 





*/

struct GBufferPack
{
    float3 Albedo;
    float3 Normal;
    float3 Ambient;
    float3 PosW;
    float Metallic;
    float Roughness;
    float Depth;
    float A0;
};

struct GBufferTarget
{
    float4 Albedo : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Ambient : SV_Target2;
    float4 MaterialProperty : SV_Target3;
    float  Depth : SV_Target4;
};

GBufferTarget BindingGBufferTarget(float3 albedo, float3 normal, float3 ambient, float metallic, float roughness, float depth)
{
    GBufferTarget target;
    target.Albedo = float4(albedo, 1.0f);
    target.Normal.xyz = (normal.xyz + 1.0f) * 0.5f;
    target.Normal.w = 1.0f;

    target.Ambient = float4(ambient, 1.0f);
    target.MaterialProperty = float4(metallic, roughness, 0.0f, 1.0f);
    target.Depth = depth;
    return target;
}
GBufferPack UnGBufferPack(float3 albedo, float3 normal, float3 ambient, float3 matproperty, float a0, float depth)
{
    GBufferPack pack;

    pack.Albedo = saturate(albedo);
    normal = normalize(normal);
    pack.Normal = normalize((normal.xyz * 2.0f) - 1.0f);
    pack.Ambient = saturate(ambient);
    pack.Metallic = saturate(matproperty.x);
    pack.Roughness = saturate(matproperty.y);
    pack.A0 = a0;
    pack.Depth = saturate(depth);

    return pack;
}
#endif