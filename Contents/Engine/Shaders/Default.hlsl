#include"Common.hlsl"





struct SceneData
{
    float4 PosW    : SV_Target0;
    float4 Albedo  : SV_Target1;
    float4 NormalW : SV_Target2;
    float MatIndex  : SV_Target3;
};
struct VS_IN
{
    float3 PosL     : POSITION;
    float3 NormalL  : NORMAL;
    float3 TangentL : TANGENT;
    float2 TexC     : TEXCOORD0;
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
    float3 fresnelR0     = matData.FresnelR0;
    float  roughness     = matData.Roughness;
    float  refractive    = matData.Refractive;
    float3 normalW       = normalize(pin.NormalW);
    float3 posW          = pin.PosW;
    float2 texC          = pin.TexC;
    float3 toEyeW = normalize(gEyePosW - posW);
    float toEyeDistance = distance(gEyePosW, posW);

    float4 ambient = float4(0.24f, 0.25f, 0.35f, 1.0f);
    if (toEyeDistance < 100.0f)
    {
        pin.SSAOPosH /= pin.SSAOPosH.w;
        diffuseAlbedo *= gTexture[gSSAOTextureIndex].Sample(gsamAnisotropicWrap, pin.SSAOPosH.xy, 0.0f).r;
    }
    diffuseAlbedo *= ambient;
    diffuseAlbedo *= gTexture[matData.TextureIndex[0]].Sample(gsamAnisotropicWrap, texC);




    
    float4 reflectionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 fresnelFactor = float3(0.0f, 0.0f, 0.0f);
    float4 reflactionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

#ifdef REFLECTION
	float3 r = reflect(-toEyeW, normalW);
	reflectionColor = gCubeMap[gCubeMapIndex].Sample(gsamLinearWrap, r);
	fresnelFactor = SchlickFresnel(fresnelR0, normalW, r);
#ifdef REFRACTION
	reflectionColor *= 0.5f;
#endif
#endif
#ifdef REFRACTION
	float3 g = refract(-toEyeW, normalW, refractive);
	fresnelFactor = SchlickFresnel(fresnelR0, normalW, g);
	reflactionColor = gCubeMap[gCubeMapIndex].Sample(gsamLinearWrap, g);
#ifdef REFLECTION
	reflactionColor *= 0.5f;
#endif
#endif

    diffuseAlbedo += float4(saturate((1.0f - roughness) * fresnelFactor * (reflectionColor.rgb + reflactionColor.rgb)), 1.0f);



	// SceneData에 직접 넣기
    Scene_Albedo  = diffuseAlbedo;
    Scene_NormalW = float4(normalize(normalW), 1.0f);
    Scene_WorldPos = float4(pin.PosW, 1.0f);


	// 최종 Data 삽입
    Output.PosW     = Scene_WorldPos;
    Output.NormalW  = (Scene_NormalW + 1.0f) * 0.5f;
    Output.Albedo   = Scene_Albedo;
    Output.MatIndex = float(gMaterialIndex + 1);
    return Output;
}