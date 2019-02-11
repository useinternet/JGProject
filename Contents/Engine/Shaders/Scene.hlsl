#include"Common.hlsl"

struct SceneData
{
    float3 PosW;
    float3 Albedo;
    float3 NormalW;
    float  Depth;
    uint MatIndex;
    uint CubIndex;
    float  IsBackGround;
};
SceneData InstallData(float2 TexC)
{
    SceneData Data;
    float4 Albedo = gTexture[gAlbedoSceneIndex].Sample(gsamAnisotropicWrap, TexC);
    Data.Albedo = Albedo.rgb;
    float4 NormalW = gTexture[gNormalSceneIndex].Sample(gsamAnisotropicWrap, TexC);
    Data.NormalW = normalize((NormalW.xyz * 2.0f) - 1.0f);
    Data.PosW = gTexture[gWorldPosSceneIndex].Sample(gsamAnisotropicWrap, TexC).xyz;
    Data.Depth = gTexture[gDepthSceneIndex].Sample(gsamAnisotropicWrap, TexC).r;
    Data.MatIndex = gTexture[gMatSceneIndex].Sample(gsamAnisotropicWrap, TexC).r - 1;
    Data.CubIndex = gTexture[gMatSceneIndex].Sample(gsamAnisotropicWrap, TexC).g - 1;
    if (Albedo.a < 0.1f)
        clip(Albedo.a - 0.1f);

    Data.IsBackGround = NormalW.a;
    return Data;
}
static const float2 gTexCoords[6] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};
 
struct VS_OUT
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD0;
};

VS_OUT VS(uint vid : SV_VertexID)
{
    VS_OUT vout;

    vout.TexC = gTexCoords[vid];


    vout.PosH = float4(2.0f * vout.TexC.x - 1.0f, 1.0f - 2.0f * vout.TexC.y, 0.0f, 1.0f);
 
    return vout;
}
float4 PS(VS_OUT pin) : SV_Target
{
    SceneData Data = InstallData(pin.TexC);
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 ao      = float3(0.05f, 0.05f, 0.1f);
    if(Data.IsBackGround > 0.1f)
    {
        MaterialData matData = gMaterialData[Data.MatIndex];

        float3 toEyeW = normalize(gEyePosW - Data.PosW);
        const float shininess = 1.0f - matData.Roughness;
        ////////////////////////////////// 라이트 계산 //////////////////////////////////////////
        float3 result = 0.0f;
        float  ShadowFactor = 1.0f;
        float4 ShadowPosH = float4(Data.PosW, 1.0f);
        float3 N = normalize(Data.NormalW);
        float3 V = toEyeW;



        float3 F0 = float3(0.04f, 0.04f, 0.04f);
        F0 = lerp(F0, Data.Albedo, matData.Metalic);
        Material m = { Data.Albedo, F0, matData.Roughness, matData.Metalic };
        if (gDirLightCount > 0)
        {

            ShadowPosH = mul(ShadowPosH, gLightData[0].ShadowTransform);
            ShadowFactor = CalcDirectionShadowFactor(ShadowPosH,
        gLightData[0].StaticShadowMapIndex,
        gLightData[0].DynamicShadowMapIndex);
    
            result += ShadowFactor * saturate(ComputeDirectionalLight(gLightData[0], m, N, V));
        }
        int i = gDirLightCount;
        for (i = gDirLightCount; i < gDirLightCount + gPointLightCount; ++i)
        {
            ShadowFactor = CalcPointShadowFactor(Data.PosW, gLightData[i]);
            result += ShadowFactor * ComputePointLight(gLightData[i], m, Data.PosW, N, V);
        }
        for (i = gDirLightCount + gPointLightCount; i < gDirLightCount + gPointLightCount + gSpotLightCount; ++i)
        {
            ShadowPosH = float4(Data.PosW, 1.0f);
            ShadowPosH = mul(ShadowPosH, gLightData[i].ShadowTransform);
            ShadowFactor = CalcSpotShadowFactor(ShadowPosH,
        gLightData[i].StaticShadowMapIndex,
        gLightData[i].DynamicShadowMapIndex);
     
            result += ShadowFactor * ComputeSpotLight(gLightData[i], m, Data.PosW, N, V);
        }
        float3 R = reflect(-toEyeW, Data.NormalW);
        float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0f), F0, matData.Roughness);
        float3 ks = F;
        float3 kd = float3(1.0f, 1.0f, 1.0f) - ks;
        kd *= 1.0f - matData.Metalic;
        float lod = 0.0f;
        if(matData.Roughness != 0.0f)
        {
            lod = matData.Roughness * 7 + 3.0f;
        }
        float3 reflection = gCubeMap[gSkyBoxIndex].SampleLevel(gsamLinearWrap, R, lod);
      //  float2 envBRDF = IntegrateBRDF(max(dot(N, V), 0.0), matData.Roughness);
        float3 specular = saturate(reflection * F); //* envBRDF.x + envBRDF.y));
        ambient.rgb = saturate((kd * Data.Albedo.rgb));

        float3 finalColor = ambient.rgb * ao + saturate(result) + specular;
      
        float3 metalic = finalColor;
        float3 plastic = finalColor;
        plastic = plastic / (plastic + float3(1.0, 1.0f, 1.0f));
        plastic = pow(plastic, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

        return float4(lerp(plastic, metalic, matData.Metalic), 1.0f);

    }
    else
    {
        if(gDirLightCount > 0)
            return float4(Data.Albedo , 1.0f);
        else
            return ambient;
    }
}