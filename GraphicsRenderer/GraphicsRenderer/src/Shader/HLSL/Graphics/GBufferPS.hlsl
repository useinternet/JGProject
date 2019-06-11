#include"GBufferVS.hlsl"
#include"GraphicsFunctions.hlsl"

float4 PS(GBufferPS pin) : SV_Target
{
    // 업로드할 데이터 정의
    float3 albedo = float3(1.0f, 1.0f, 1.0f);
    float3 normal = normalize(pin.NormalW);
    float3 tangent = normalize(pin.TangentW);
    float3 specular = float3(1.0f, 1.0f, 1.0f);
    float depth = pin.Depth;
    float metallic = 0.99f;
    float roughness = 0.99f;
    float3 A0 = float3(1.0f, 1.0f, 1.0f);

// 디퓨즈 텍스쳐
#ifdef USE_ALBEDO
    albedo = gTexture[GBUFFER_ALBEDO].Sample(gAnisotropicWrap, pin.TexC).rgb;
#endif
// 노멀 법선 텍스쳐
#ifdef USE_NORMAL
    normal = CalcBumpNormal(gTexture[GBUFFER_NORMAL].Sample(gAnisotropicWrap, pin.TexC).rgb, normal, tangent);
#endif
   // 스펙큘러 텍스쳐
#ifdef USE_SPECULAR
    specular = gTexture[GBUFFER_SPECULAR].Sample(gAnisotropicWrap, pin.TexC).rgb;
#endif
    // 메탈릭 텍스쳐
#ifdef USE_METALLIC
    metallic = gTexture[GBUFFER_METALLIC].Sample(gAnisotropicWrap, pin.TexC).r;
#endif
    // 거칠기 텍스쳐
#ifdef USE_ROUGHNESS
    roughness = gTexture[GBUFFER_ROUGHNESS].Sample(gAnisotropicWrap, pin.TexC).r;
#endif
    // 오쿨루션 텍스쳐
#ifdef USE_A0
    A0 = gTexture[GBUFFER_A0].Sample(gAnisotropicWrap, pin.TexC).rgb;
#endif


    // -- 라이트 관련 데이터 정의 (임시) --
    float3 dirLighting = float3(0.0f, 0.0f, 0.0f);
    float3 ambientLighting = float3(0.0f, 0.0f, 0.0f);
    float3 Light[3] =
    {
        float3(-1.0f, 0.0f, 0.0f),
        float3(1.0f, 0.0f, 0.0f),
        float3(0.0f, -1.0f, 0.0f)
    };

    // -- pbr 구현 -> 필요한 공식 재료 
    float3 V = normalize(gPassCB.EyePos.xyz - pin.PosW.xyz);
    float3 N = normal;
    float NDotV = saturate(dot(N, V));
    float3 R = reflect(-V, N);

    float Fdielectric = 0.04f;
    float3 F0 = lerp(Fdielectric.xxx, albedo.xyz, metallic);

    // -- pbr구현 -> 라이팅 계산
    for (int i = 0; i < 1; ++i)
    {
        float3 L = -Light[i];
        float3 H = normalize(L + V);


        float NDotL = saturate(dot(N, L));
        float NDotH = saturate(dot(N, H));
        float VDotH = saturate(dot(V, H));
        float3 F = fresnelSchlick(F0, VDotH);
        float D = ndfGGX(NDotH, roughness);
        float G = gaSchlickGGX(NDotL, NDotV, roughness);


        float3 kd = lerp(float3(1, 1, 1) - F, float3(0.0f, 0.0f, 0.0f), metallic);


        // Lambert diffuse BRDF.
        float3 diffuseBRDF = kd * albedo.xyz;

        // Cook-Torrance specular microfacet BRDF.
        float3 specularBRDF = (F * D * G) / max(0.00001f, 4.0f * NDotL * NDotV);


        dirLighting += (diffuseBRDF + specularBRDF) * NDotL;

    }
    // --IBL 구현 ambientLight 계산
    {
        
        float3 irradiance = gCubeTexture[GBUFFER_IRRMAP].Sample(gLinearWrap, N).rgb;
        //
    
        float3 F = fresnelSchlick(F0, NDotV);

        float3 kd = lerp(1.0f - F, 0.0f, metallic);
        float3 diffuseIBL = kd * albedo.rgb * irradiance;

        uint width, height, levels;
        gCubeTexture[GBUFFER_SPECMAP].GetDimensions(0, width, height, levels);
        uint specularTextureLevels = levels;
        float3 specularIrradiance = gCubeTexture[GBUFFER_SPECMAP].SampleLevel(gLinearWrap, R, roughness * specularTextureLevels).rgb;


        float2 specularBRDF = gTexture[GBUFFER_SPECBRDF].Sample(gAnisotropicWrap, float2(NDotV, roughness)).rg;

        float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
        ambientLighting = diffuseIBL + specularIBL;

    }

    
    return float4(dirLighting + ambientLighting, 1.0f);
}