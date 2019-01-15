#include "Common.hlsl"

struct SceneData
{
    float4 PosW : SV_Target0;
    float4 Albedo : SV_Target1;
    float4 NormalW : SV_Target2;
    float MatIndex : SV_Target3;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float2 TexC : TEXCOORD0;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

	// 국소 정점 위치를 입방체 맵 조회 벡터로 사용
    vout.PosL = vin.PosL;

	// 세계 공간 으로 변환
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	// 하늘구의 중심을 항상 카메라 위치에 둔다.
    posW.xyz += gEyePosW;

	// z/w = 1이 되도록 z = w 로한다. 
    vout.PosH = mul(posW, gViewProj).xyww;

    return vout;
}

SceneData PS(VertexOut pin) : SV_Target
{
    SceneData Output;
    Output.PosW     = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Output.NormalW  = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Output.Albedo   = gCubeMap[gSkyBoxIndex].Sample(gsamLinearWrap, pin.PosL);
    Output.MatIndex = 0.0f;
    return Output;
}