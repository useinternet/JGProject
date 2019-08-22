#include"GraphicsStructDefine.hlsl"
#ifndef __GRAPHICSFUNCTIONS_HLSL__
#define __GRAPHICSFUNCTIONS_HLSL__
/* 단순 계산을 위한 함수 */
float3 CalcBumpNormal(float3 normalMap, float3 normalW, float3 tangentW)
{
    float3 normalT = 2.0f * normalMap - 1.0f;

    float3 N = normalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

    float3 bumpedNormalW = mul(normalT, TBN);

    return normalize(bumpedNormalW);
}

float3 CalcWorldPosition(float2 texC, float depth, float4x4 invViewProj)
{
    float z = depth;

    float x = 2.0f * texC.x - 1.0f;
    float y = 1.0f - 2.0f * texC.y;
    float4 PosInProj = float4(x, y, z, 1.0f);
    float4 PosInWorld = mul(PosInProj, invViewProj);
    PosInWorld.xyz /= PosInWorld.w;
    return PosInWorld.xyz;
}
float ConvertLinearDepth(float depth, float farZ, float nearZ)
{
    float proj_A = farZ / (farZ - nearZ);
    float proj_B = (-nearZ) / (farZ - nearZ);
    return proj_B / (depth - proj_A);
}
/* BRDF을 위한 공식 함수 */
float ndfGGX(float NDotH, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (NDotH * NDotH) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (_PI * denom * denom);
}

float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

float gaSchlickGGX(float NDotL, float NDotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(NDotL, k) * gaSchlickG1(NDotV, k);
}

float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


float3 PBR()
{
    // V = normalize(Eye - PosW)
    // NDotV = saturate(dot(N, Lo))
    // R = 2.0f * cosLo * N - Lo;
    // F0 = lerp(float3(0.04f,0.04f,0.04f), albedo.xyz, metallic);





    return float3(0.0f, 0.0f, 0.0f);
}

#endif