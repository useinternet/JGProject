
#include"GraphicsDefine.hlsl"

#ifndef _SKYBOX_HLSL__
#define _SKYBOX_HLSL__
struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
    VertexOut vout;
    ObjectCB obj = gObjects[instanceID];
    float4x4 world = obj.World;
    
	// 국소 정점 위치를 입방체 맵 조회 벡터로 사용
    vout.PosL = vin.PosL;

	// 세계 공간 으로 변환
    float4 posW = mul(float4(vin.PosL, 1.0f), world);

	// 하늘구의 중심을 항상 카메라 위치에 둔다.
    posW.xyz += gToEye;

	// z/w = 1이 되도록 z = w 로한다. 
    vout.PosH = mul(posW, gViewProj).xyww;

    return vout;
}

#endif