
#include "Common.hlsl"

struct VertexIn
{ 
	float3 PosL     : POSITION;
	float3 NormalL  : NORMAL;
	float3 TangentL : TANGENT;
	float2 TexC     : TEXCOORD0;
};

struct VertexOut
{
	float4 PosH       : SV_POSITION;
	float4 SSAOPosH   : POSITION0;
	float3 PosW       : POSITION1;
	float3 NormalW    : NORMAL;
	float3 TangentW   : TANGENT;
	float2 TexC       : TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	MaterialData matData = gMaterialData[gMaterialIndex];
	// Transform to homogeneous clip space.
	float4 PosW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = PosW.xyz;

	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
	vout.TangentW = mul(vin.TangentL, (float3x3)gWorld);

	vout.PosH = mul(PosW, gViewProj);
	vout.SSAOPosH = mul(PosW, gViewProjTex);

	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;

	return vout;
}
float4 PS(VertexOut pin) : SV_Target
{
	// 머터리얼 데이터 읽기
	MaterialData matData = gMaterialData[gMaterialIndex];
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
	float3 fresnelR0 = matData.FresnelR0;
	float roughness = matData.Roughness;
	float refractive = matData.Refractive;
	float3 normalW = normalize(pin.NormalW);
	float3 posW = pin.PosW;
	float2 texC = pin.TexC;
	// 텍스쳐 읽기 //
	diffuseAlbedo *= gTexture[matData.TextureIndex[0]].Sample(gsamAnisotropicWrap, texC);

	float4 normalMap = gTexture[matData.TextureIndex[1]].Sample(gsamAnisotropicWrap, texC);
	float3 bumpNormalW = NormalSampleToWorldSpace(normalMap.rgb, normalW, pin.TangentW);
	normalW = normalize(bumpNormalW);


	// SSAO 계산
	pin.SSAOPosH /= pin.SSAOPosH.w;
	float AmbientAccess = gTexture[gSSAOTextureIndex].Sample(gsamLinearClamp, pin.SSAOPosH.xy, 0.0f).r;
	// 카메라 벡터 // 주변광 
	float3 toEyeW = normalize(gEyePosW - posW);
	float4 ambient = AmbientAccess * gAmbientLight * diffuseAlbedo;


	// 라이트 계산
	const float shininess = 1.0f - roughness;
	Material mat = { diffuseAlbedo, fresnelR0, shininess };
	
	// 다이렉트 라이트 만 계산
	float3 result = 0.0f;
	float  shadowFactor = 1.0f;
	float4 ShadowPosH;
	if (gDirLightCount > 0)
	{
		ShadowPosH = float4(pin.PosW, 1.0f);
		ShadowPosH = mul(ShadowPosH, gLightData[0].ShadowTransform);
		shadowFactor = saturate(CalcShadowFactor(ShadowPosH, gLightData[0].ShadowMapIndex));
		result += saturate(shadowFactor * ComputeDirectionalLight(gLightData[0], mat, normalW, toEyeW));
	}
	uint i = gDirLightCount;
	for (i = gDirLightCount; i < gDirLightCount + gPointLightCount; ++i)
	{
		shadowFactor = CalcPointShadowFactor(posW, gLightData[i]);
		result +=  saturate(shadowFactor * ComputePointLight(gLightData[i], mat, posW, normalW, toEyeW));
	}
	for (i = gDirLightCount + gPointLightCount; i < gDirLightCount + gPointLightCount + gSpotLightCount; ++i)
	{
		ShadowPosH = float4(pin.PosW, 1.0f);
		ShadowPosH = mul(ShadowPosH, gLightData[i].ShadowTransform);
		shadowFactor = CalcSpotShadowFactor(ShadowPosH, gLightData[i].ShadowMapIndex);
		result += saturate(shadowFactor * ComputeSpotLight(gLightData[i], mat,posW, normalW, toEyeW));
	}
	float4 directLight = float4(result, 0.0f);
	float4 litColor = ambient + saturate(directLight);

///////////////////////////////////////////////////
///////////////  굴절 및 반사 계산   ///////////////
	float4 reflectionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 fresnelFactor   = float3(0.0f, 0.0f, 0.0f);
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
///////////////////////////////////////////////////
///////////////////////////////////////////////////
	litColor.rgb += saturate(shininess * fresnelFactor * (reflectionColor.rgb + reflactionColor.rgb));
	litColor.a = diffuseAlbedo.a;
	return directLight;
	return float4(gLightData[5].Strength, 1.0f);
	return litColor;
}