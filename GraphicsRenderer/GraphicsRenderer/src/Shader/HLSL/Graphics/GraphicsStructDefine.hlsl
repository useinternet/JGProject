
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
    float padding;
};
/* GBuffer 데이터 





*/

struct GBufferPack
{
    float4 Albedo_Metallic; // format = //
    float4 Normal_Roughness;
    float4 Specular_Depth;
    float4 A0;
};

#endif