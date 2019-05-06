#include"SkyBoxVS.hlsl"



float4 PS(VertexOut pin) : SV_Target
{
    float3 N = normalize(pin.PosL);
    float3 R = N;
    float3 V = R;
    float  roughness = padding.x;
    float3 PrefilteredColor = float3(0.0f, 0.0f, 0.0f);
    float totalWeight = 0.0f;
	
    const uint NumSamples = 1024;
    for (uint i = 0; i < NumSamples; i++)
    {
        float2 Xi = Hammersley(i, NumSamples);
        float3 H = ImportanceSampleGGX(Xi, N, roughness);
        float3 L = 2 * dot(V, H) * H - V;
        float NdotL = saturate(dot(N, L));
        if (NdotL > 0)
        {
            PrefilteredColor += gCubeMap[0].SampleLevel(gLinearWrapSampler, L, 0).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    PrefilteredColor /= totalWeight;

    return float4(PrefilteredColor, 1.0f);
}