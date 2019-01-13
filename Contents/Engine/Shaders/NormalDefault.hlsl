#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

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
	float4 PosH     : SV_POSITION;
	float3 PosW     : POSITION;
	float3 NormalW  : NORMAL;
	float2 TexC     : TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	MaterialData matData = gMaterialData[gMaterialIndex];
	// Transform to homogeneous clip space.
	float4 PosW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = PosW.xyz;

	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	vout.PosH = mul(PosW, gViewProj);


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
	// 텍스쳐 읽기
	diffuseAlbedo *= gTexture[matData.TextureIndex[0]].Sample(gsamAnisotropicWrap, pin.TexC);

	// 노멀 벡터 정규화
	pin.NormalW = normalize(pin.NormalW);

	// 카메라 벡터 // 주변광 
	float3 toEyeW = normalize(gEyePosW - pin.PosW);
	float4 ambient = gAmbientLight * diffuseAlbedo;

	// 라이트 계산
	const float shininess = 1.0f - matData.Roughness;
	Material mat = { diffuseAlbedo, matData.FresnelR0, shininess };
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW, pin.NormalW, toEyeW, 1.0f);

	float4 litColor = ambient + directLight;

	
	// Add in specular reflections.
	float3 r = reflect(-toEyeW, pin.NormalW);
	float3 g = refract(-toEyeW, pin.NormalW, 0.79);
	float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
	float3 fresnelFactor = SchlickFresnel(fresnelR0, pin.NormalW, r);
	litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;

	litColor.a = diffuseAlbedo.a;

	return litColor;
}