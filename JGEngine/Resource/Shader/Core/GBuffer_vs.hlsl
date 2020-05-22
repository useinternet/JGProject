
struct GameObject
{
    float4x4 world;
};

StructuredBuffer<GameObject> gGameObjects : register(t0,space0);


cbuffer CameraCB : register(b0, space0)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
}


struct VS_In
{
    float3 posL     : POSITION;
    float3 normalL   : NORMAL;
    float3 tangentL  : TANGENT;
    float2 texCoord : TEXCOORD;
};


struct VS_Out
{
	float4 posH     : SV_POSITION;
    float3 posW     : POSITION;
    float3 normalW  : NORMAL;
    float3 tangentW : TANGENT;
    float2 texCoord : TEXCOORD;
};


VS_Out main(VS_In vin , uint instanceID : SV_InstanceID)
{
	VS_Out vout;
	// Transform to homogeneous clip space.
    float4x4 world = gGameObjects[instanceID].world;
    float4 posW = mul(float4(vin.posL, 1.0f), world);

    vout.posH = mul(posW, gViewProj);
    vout.posW = posW.xyz;
    vout.normalW = normalize(mul(vin.normalL, (float3x3)world));
    vout.tangentW = normalize(mul(vin.tangentL, (float3x3)world));
    vout.texCoord = vin.texCoord;
    return vout;
}