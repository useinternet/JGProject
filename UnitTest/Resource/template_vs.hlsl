

StructuredBuffer<float4x4> gWorldViewProj : register(t0, space0);




struct VS_In
{
    float3 posL     : POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 texCoord : TEXCOORD;
};


struct VS_Out
{
	float4 PosH  : SV_POSITION;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float2 texCoord : TEXCOORD;
};

VS_Out vs(VS_In vin , uint instanceID : SV_InstanceID)
{
	VS_Out vout;
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.posL, 1.0f), gWorldViewProj[instanceID]);
    vout.texCoord = vin.texCoord;
    return vout;
}
