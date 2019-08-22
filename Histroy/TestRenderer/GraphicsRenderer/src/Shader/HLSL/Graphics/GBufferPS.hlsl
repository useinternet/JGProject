#include"GBufferVS.hlsl"
#include"GraphicsFunctions.hlsl"






GBufferTarget PS(GBufferPS pin) //: SV_Target
{
    // 업로드할 데이터 정의
    float3 albedo = float3(1.0f, 1.0f, 1.0f);
    float3 normal = normalize(pin.NormalW);
    float3 tangent = normalize(pin.TangentW);
    float3 ambient = float3(0.15f, 0.15f, 0.15f);
    float3 specular = float3(1.0f, 1.0f, 1.0f);
    float  depth = pin.Depth;
    float  metallic = 0.5f;
    float  roughness = 0.99f;
    float3 A0 = float3(1.0f, 1.0f, 1.0f);



    // 디퓨즈 텍스쳐
#ifdef USE_SLOT0
    albedo = gTexture[0].Sample(gAnisotropicWrap, pin.TexC).rgb;
#endif


    // 노멀 텍스쳐
#ifdef USE_SLOT1
     normal = CalcBumpNormal(gTexture[1].Sample(gAnisotropicWrap, pin.TexC).rgb, normal, tangent);
#endif

        // 스펙큘러 텍스쳐
#ifdef USE_SLOT2
     specular = gTexture[2].Sample(gAnisotropicWrap, pin.TexC).rgb;
#endif

        // 메탈릭 텍스쳐
#ifdef USE_SLOT3
     metallic = gTexture[3].Sample(gAnisotropicWrap, pin.TexC).r;
#endif

        // 거칠기 텍스쳐
#ifdef USE_SLOT4
     roughness = gTexture[4].Sample(gAnisotropicWrap, pin.TexC).r;
#endif

        // A0 텍스쳐
#ifdef USE_SLOT5
    A0 = gTexture[5].Sample(gAnisotropicWrap, pin.TexC).rgb;
#endif

    // irr = cube_1_slot  spmap = cube_0_slot  brdf = tex_6_slot
#ifdef USE_IBL
        float3 V = normalize(gPassCB.EyePos.xyz - pin.PosW.xyz);
        float3 N = normal;
        float NDotV = saturate(dot(N, V));
        float3 R = reflect(-V, N);

        float Fdielectric = 0.04f;
        float3 F0 = lerp(Fdielectric.xxx, albedo.xyz, metallic);
        float3 irradiance = gCubeTexture[1].Sample(gLinearWrap, N).rgb;
        //
    
        float3 F = fresnelSchlick(F0, NDotV);

        float3 kd = lerp(1.0f - F, 0.0f, metallic);
        float3 diffuseIBL = kd * albedo.rgb * irradiance;

        uint width, height, levels;
        gCubeTexture[0].GetDimensions(0, width, height, levels);
        uint specularTextureLevels = levels;
        float3 specularIrradiance = gCubeTexture[0].SampleLevel(gLinearWrap, R, roughness * specularTextureLevels).rgb;


        float2 specularBRDF = gTexture[6].Sample(gAnisotropicWrap, float2(NDotV, roughness)).rg;

        float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
        ambient = diffuseIBL + specularIBL;
#endif
    return BindingGBufferTarget(albedo, normal, ambient, metallic, roughness, depth);

    /*
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

    
    return float4(dirLighting + ambientLighting, 1.0f);*/
}