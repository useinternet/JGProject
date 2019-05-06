#include"SkyBoxVS.hlsl"


// 1. 오브젝트 노멀 값
// 2. 오브젝트 기준 큐브맵
float4 PS(VertexOut pin) : SV_Target
{
    float3 normal = pin.PosL;
    float3 irradiance = float3(0.0f, 0.0f, 0.0f);


    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = cross(up, normal);


    up = cross(normal, right);
    float sampleDelta = 0.025f;
    float nrSamples = 0.0f;

    for (float phi = 0.0f; phi < _2PI; phi += sampleDelta)
    {
        for (float theta = 0.0f; theta < _PIDIV2; theta += sampleDelta)
        {
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            float3 sampleVec = (tangentSample.x * right) + (tangentSample.y * up) + (tangentSample.z * normal);

#ifdef USE_CUBETEXTURE_SLOT0
            irradiance += gCubeMap[0].Sample(gLinearWrapSampler, sampleVec).rgb * cos(theta) * sin(theta);
#endif
            nrSamples++;
        }

    }
    irradiance = _PI * irradiance * (1 / (float) nrSamples);


    return float4(irradiance, 1.0f);
}