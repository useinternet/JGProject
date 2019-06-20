#include"LightingPassVS.hlsl"
#include"RS_GCommon.hlsl"
#include"GraphicsFunctions.hlsl"

/*
			Normal,
			Ambient,
			MaterialProperty,
			A0,
			Depth,
*/
#define SKY 0
#define ALBEDO 0 
#define NORMAL 1
#define AMBIENT 2
#define MATERIAL 3
#define A0 4
#define DEPTH 5

float4 PS(QuadPS pin) : SV_TARGET
{
    float depth = gTexture[DEPTH].Sample(gLinearWrap, pin.TexC).r;


    if (depth == 1.0f)
    {
        float3 v = normalize(CalcWorldPosition(pin.TexC, depth, gPassCB.InvViewProj));
        return gCubeTexture[SKY].SampleLevel(gLinearWrap, v, 0.0f);
    }


    float3 albedo = gTexture[ALBEDO].Sample(gLinearWrap, pin.TexC).rgb;
    float3 normal = gTexture[NORMAL].Sample(gLinearWrap, pin.TexC).rgb;
    float3 ambient = gTexture[AMBIENT].Sample(gLinearWrap, pin.TexC).rgb;
    float3 matProperty = gTexture[MATERIAL].Sample(gLinearWrap, pin.TexC).rgb;
 


    GBufferPack pack;
    pack = UnGBufferPack(albedo, normal, ambient, matProperty, 0.0f, depth);
    float3 WorldPos = CalcWorldPosition(pin.TexC, depth, gPassCB.InvViewProj);
 
    




    float3 dirLighting = float3(0.0f, 0.0f, 0.0f);
    float3 Light[3] =
    {
        float3(-1.0f, 0.0f, 0.0f),
        float3(1.0f, 0.0f, 0.0f),
        float3(0.0f, -1.0f, 0.0f)
    };

    float3 V = normalize(gPassCB.EyePos.xyz - WorldPos);
    float3 N = normalize(pack.Normal);
    float NDotV = saturate(dot(N, V));
    float3 R = normalize(reflect(-V, N));

    float Fdielectric = 0.04f;
    float3 F0 = lerp(Fdielectric.xxx, pack.Albedo.xyz, pack.Metallic);


    for (int i = 0; i < 1; ++i)
    {
        float3 L = -Light[i];
        float3 H = normalize(L + V);


        float NDotL = saturate(dot(N, L));
        float NDotH = saturate(dot(N, H));
        float VDotH = saturate(dot(V, H));
        float3 F = fresnelSchlick(F0, VDotH);
        float D = ndfGGX(NDotH, pack.Roughness);
        float G = gaSchlickGGX(NDotL, NDotV, pack.Roughness);


        float3 kd = lerp(float3(1, 1, 1) - F, float3(0.0f, 0.0f, 0.0f), pack.Metallic);


        // Lambert diffuse BRDF.
        float3 diffuseBRDF = kd * pack.Albedo.xyz;

        // Cook-Torrance specular microfacet BRDF.
        float3 specularBRDF = (F * D * G) / max(0.00001f, 4.0f * NDotL * NDotV);


        dirLighting += (diffuseBRDF + specularBRDF) * NDotL;
    }





    return float4(dirLighting + pack.Ambient, 1.0f);
}