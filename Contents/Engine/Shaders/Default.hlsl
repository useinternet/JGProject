#include"Common.hlsl"
#include"CommonInputLayout.hlsl"




struct SceneData
{
    float4 PosW    : SV_Target0;
    float4 Albedo  : SV_Target1;
    float4 NormalW : SV_Target2;
    float2 DataIndex  : SV_Target3;
};



struct VS_OUT
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION0;
    float4 SSAOPosH : POSITION1;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexC : TEXCOORD0;
};


VS_OUT VS(VS_IN vin)
{
    VS_OUT vout;
    MaterialData matData = gMaterialData[gMaterialIndex];

#ifdef SKINNED
    float weight[4] = {0.0f,0.0f,0.0f,0.0f};
    weight[0] = vin.BoneWeights.x;
    weight[1] = vin.BoneWeights.y;
    weight[2] = vin.BoneWeights.z;
    weight[3] = 1.0f - (weight[0] + weight[1] + weight[2]);

    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);
    float3 tangentL = float3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; ++i)
    {
        posL += weight[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneID[i]]).xyz;
        normalL += weight[i] * mul(vin.NormalL, (float3x3)gBoneTransforms[vin.BoneID[i]]);
        tangentL += weight[i] * mul(vin.TangentL.xyz, (float3x3)gBoneTransforms[vin.BoneID[i]]);
    }
   vin.PosL     = posL;
   vin.NormalL  = normalL;
   vin.TangentL = tangentL;
#endif
 


    float4 PosW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = PosW.xyz;
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);
    vout.TangentW = mul(vin.TangentL, (float3x3) gWorld);
    vout.PosH = mul(PosW, gViewProj);
    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
    vout.TexC = mul(texC, matData.MatTransform).xy;
    vout.SSAOPosH = mul(PosW, gViewProjTex);
    return vout;
}


SceneData PS(VS_OUT pin)
{
	// 각 정보 초기화
    SceneData Output;
    MaterialData matData = gMaterialData[gMaterialIndex];

    float4 Scene_Albedo   = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 Scene_NormalW  = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 Scene_WorldPos = float4(0.0f, 0.0f, 0.0f, 1.0f);

	//
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
    float3 normalW       = normalize(pin.NormalW);
    float3 posW          = pin.PosW;
    float2 texC          = pin.TexC;
    float3 toEyeW = normalize(gEyePosW - posW);
    float  toEyeDistance = distance(gEyePosW, posW);


    if (toEyeDistance < 10000.0f)
    {
        pin.SSAOPosH /= pin.SSAOPosH.w;
        diffuseAlbedo *= gTexture[gSSAOTextureIndex].Sample(gsamAnisotropicWrap, pin.SSAOPosH.xy, 0.0f).r;
    }
  
#ifdef USING_DIFFUSEMAP
    diffuseAlbedo *= gTexture[matData.TextureIndex[0]].Sample(gsamAnisotropicWrap, texC);
#endif
#ifdef USING_NORMALMAP
    float4 normalMap = gTexture[matData.TextureIndex[1]].Sample(gsamAnisotropicWrap, texC);
    float3 bumpNormal = NormalSampleToWorldSpace(normalMap.rgb, normalW, pin.TangentW);
    normalW = normalize(bumpNormal);
#endif
	// SceneData에 직접 넣기
    Scene_Albedo = diffuseAlbedo;
    Scene_NormalW = float4(normalize(normalW), 1.0f);
    Scene_WorldPos = float4(pin.PosW, 1.0f);


	// 최종 Data 삽입
    Output.PosW     = Scene_WorldPos;
    Output.NormalW  = (Scene_NormalW + 1.0f) * 0.5f;
    Output.Albedo = Scene_Albedo;
    Output.DataIndex = float2(gMaterialIndex + 1, gCubeMapIndex + 1);

    return Output;
}