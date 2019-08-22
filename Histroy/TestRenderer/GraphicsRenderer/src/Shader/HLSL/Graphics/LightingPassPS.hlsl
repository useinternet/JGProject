#include"QuadVS.hlsl"
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
#define SHADOWMAP 6


float3 CalcDiectionalLighting(LightCB light, float3 N, float3 V,float3 F0, float3 Albedo, float Roughness, float Metallic)
{
    float3 color = float3(0.0f, 0.0f, 0.0f);
    float NDotV = saturate(dot(N, V));
    float3 L = normalize(-light.direction);
    float3 H = normalize(L + V);

    float NDotL = saturate(dot(N, L));
    float NDotH = saturate(dot(N, H));
    float VDotH = saturate(dot(V, H));


    float3 F = fresnelSchlick(F0, VDotH);
    float D = ndfGGX(NDotH, Roughness);
    float G = gaSchlickGGX(NDotL, NDotV, Roughness);

    float3 kd = lerp(float3(1, 1, 1) - F, float3(0.0f, 0.0f, 0.0f), Metallic);


        // Lambert diffuse BRDF.
    float3 diffuseBRDF = kd * Albedo;

        // Cook-Torrance specular microfacet BRDF.
    float3 specularBRDF = (F * D * G) / max(0.00001f, 4.0f * NDotL * NDotV);

    color += (diffuseBRDF + specularBRDF) * NDotL;

    return color * light.color * light.indensity;
}
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
    float shadowFactor = gTexture[SHADOWMAP].Sample(gLinearWrap, pin.TexC).r;


    GBufferPack pack;
    pack = UnGBufferPack(albedo, normal, ambient, matProperty, 0.0f, depth);
    float3 WorldPos = CalcWorldPosition(pin.TexC, depth, gPassCB.InvViewProj);
 
    




    //float3 dirLighting = float3(0.0f, 0.0f, 0.0f);
    //float3 Light[3] =
    //{
    //    float3(-1.0f, 0.0f, 0.0f),
    //    float3(1.0f, 0.0f, 0.0f),
    //    float3(0.0f, -1.0f, 0.0f)
    //};

    float3 V = normalize(gPassCB.EyePos.xyz - WorldPos);
    float3 N = normalize(pack.Normal);
    float NDotV = saturate(dot(N, V));
    float3 R = normalize(reflect(-V, N));

    float Fdielectric = 0.04f;
    float3 F0 = lerp(Fdielectric.xxx, pack.Albedo.xyz, pack.Metallic);

    
    float3 lit = float3(0.0f, 0.0f, 0.0f);

    for (uint i = 0; i < gPassCB.LightCount; ++i)
    {
        // directional light
        if(gLightCBs[i].type == 0)
        {
            lit = CalcDiectionalLighting(gLightCBs[0], N, V, F0, pack.Albedo.xyz, pack.Roughness, pack.Metallic);
        }
        // spot light
        if (gLightCBs[i].type == 1)
        {
           
        }
        // point light
        if (gLightCBs[i].type == 2)
        {
           
        }
    }

        


    lit *= shadowFactor.r;
    lit += pack.Ambient;

    return float4(lit, 1.0f);
}