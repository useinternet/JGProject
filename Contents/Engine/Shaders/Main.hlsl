

#include"GraphicsDefine.hlsl"




struct ToVs
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float2 TexC : TEXCOORD;
#ifdef USE_SKINNED
    float4 BoneWeights : WEIGHTS;
    uint4  BoneID      : BONEID;
#endif
};
struct VsToPs
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexC : TEXCOORD;
};
VsToPs VS(ToVs vin, uint instanceID : SV_InstanceID)
{
    VsToPs output;
    ObjectCB obj = gObjects[instanceID];
    float4x4 world = obj.World;


#ifdef USE_SKINNED
    float3 posL = float3(0.0f,0.0f,0.0f);
    float3 normalL = float3(0.0f,0.0f,0.0f);
    float3 tangentL = float3(0.0f,0.0f,0.0f);

    for(int i =0; i<4; ++i)
    {
       posL += vin.BoneWeights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransform[vin.BoneID[i]]).xyz;
       normalL += vin.BoneWeights[i] * mul(vin.NormalL, (float3x3)gBoneTransform[vin.BoneID[i]]);
       tangentL += vin.BoneWeights[i] * mul(vin.TangentL, (float3x3)gBoneTransform[vin.BoneID[i]]);
    }
    vin.PosL     = posL;
    vin.NormalL  = normalL;
    vin.TangentL = tangentL;
#endif



    float4 PosW = mul(float4(vin.PosL, 1.0f), world);
    output.PosH = mul(PosW, gViewProj);
    output.PosW = PosW;
    output.NormalW = mul(vin.NormalL, (float3x3)world);
    output.TangentW = mul(vin.TangentL, (float3x3)world);
    output.TexC = vin.TexC;
    return output;
}
static const float3 SunDir = { 0.0f, 0.0f, 1.0f };



float4 PS(VsToPs pin) : SV_Target
{
    float3 N = normalize(pin.NormalW);
    float3 T = normalize(pin.TangentW);
    float3 L = normalize(-SunDir);

    float NDotL = dot(N, L);
 
    float3 Surface = float3(1.0f, 1.0f, 1.0f);
    float4 TextureColor = gDiffuseTexture.Sample(gAnisotropicWrapSampler, pin.TexC);

    return TextureColor * NDotL;
}
