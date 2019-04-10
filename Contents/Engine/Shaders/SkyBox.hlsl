
#include"GraphicsDefine.hlsl"


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


GBufferPack PS(VertexOut pin) : SV_Target
{
    GBufferPack gbuffer;
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
#ifdef USE_CUBETEXTURE_SLOT0
    color = gCubeMap[0].Sample(gLinearWrapSampler, pin.PosL);
#endif


    gbuffer.Albedo = float4(color.xyz, 0.0f);
    gbuffer.Normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    gbuffer.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    gbuffer.Depth = 1.0f;
    return gbuffer;
}