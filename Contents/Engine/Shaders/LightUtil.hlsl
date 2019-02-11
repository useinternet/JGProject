#include"BSDF.hlsl"
struct Light
{
    float3 Strength;
    float FalloffStart; // point/spot light only
    float3 Direction; // directional/spot light only
    float FalloffEnd; // point/spot light only
    float3 Position; // point light only
    float SpotPower; // spot light only
    float4x4 ShadowTransform;
    uint StaticShadowMapIndex;
    uint DynamicShadowMapIndex;
    float CosOuterAngle;
    float CosInnerAngle;
};

struct Material
{
    float3 DiffuseAlbedo;
    float3 F0;
    float  Roughness;
    float Metalic;
};

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Linear falloff.
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));

    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

    return reflectPercent;
}
//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for directional lights.
//---------------------------------------------------------------------------------------
float3 ComputeDirectionalLight(Light l, Material mat,  float3 N, float3 V)
{
    float3 L = normalize(-l.Direction);
    float3 H = normalize(V + L);
    float NDF = DistributionGGX(N, H, mat.Roughness);
    float G = GeometrySmith(N, V, L, mat.Roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0.0f), mat.F0);

    float3 ks = F;
    float3 kd = float3(1.0f, 1.0f, 1.0f) - ks;
    kd *= 1.0f - mat.Metalic;


    float3 numerator = NDF * G * F;
    float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f);
    float3 specular = numerator / max(denominator, 0.001);

    float NdotL = max(dot(N, L), 0.0f);

    return (kd * mat.DiffuseAlbedo / JGPI + specular) * l.Strength * NdotL;
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for point lights.
//---------------------------------------------------------------------------------------
float3 ComputePointLight(Light l, Material mat, float3 pos, float3 N, float3 V)
{
    float3 L = normalize(l.Position - pos);
    float  D = length(l.Position - pos);
    float3 H = normalize(V + L);
    float NDF = DistributionGGX(N, H, mat.Roughness);
    float G = GeometrySmith(N, V, L, mat.Roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0.0f), mat.F0);

    float3 ks = F;
    float3 kd = float3(1.0f, 1.0f, 1.0f) - ks;
    kd *= 1.0f - mat.Metalic;

    float3 numerator = NDF * G * F;
    float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f);
    float3 specular = numerator / max(denominator, 0.001);
    float NdotL = max(dot(N, L), 0.0f);



    // Attenuate light by distance.
    float att = CalcAttenuation(D, l.FalloffStart, l.FalloffEnd);
    

    return (kd * mat.DiffuseAlbedo / JGPI + specular) * l.Strength * NdotL * (att * l.SpotPower);
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for spot lights.
//---------------------------------------------------------------------------------------
float3 ComputeSpotLight(Light l, Material mat, float3 pos, float3 N, float3 V)
{
    // The vector from the surface to the light.
    float3 L = normalize(l.Position - pos);

    // The distance from surface to light.
    float D = length(l.Position - pos);

    // Range test.
    if (D > l.FalloffEnd)
        return 0.0f;

    // Scale light down by Lambert's cosine law.
    float ndotl = max(dot(L, N), 0.0f);
    float3 lightStrength = l.Strength * ndotl;

    // Attenuate light by distance.
    float att = CalcAttenuation(D, l.FalloffStart, l.FalloffEnd);
    lightStrength *= att;

    // Scale by spotlight
    float cosangle = dot(l.Direction, -L);
    float conAtt = saturate((cosangle - l.CosOuterAngle) / (l.CosInnerAngle - l.CosOuterAngle));
    lightStrength *= (conAtt * conAtt * l.SpotPower);


   
    float3 H = normalize(V + L);
    float NDF = DistributionGGX(N, H, mat.Roughness);
    float G = GeometrySmith(N, V, L, mat.Roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0.0f), mat.F0);

    float3 ks = F;
    float3 kd = float3(1.0f, 1.0f, 1.0f) - ks;
    kd *= 1.0f - mat.Metalic;

    float3 numerator = NDF * G * F;
    float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f);
    float3 specular = numerator / max(denominator, 0.001);

    return (kd * mat.DiffuseAlbedo / JGPI + specular) * lightStrength;
}