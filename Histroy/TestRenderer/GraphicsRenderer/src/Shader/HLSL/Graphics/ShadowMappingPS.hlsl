#include"QuadVS.hlsl"
#include"GraphicsStructDefine.hlsl"
#include"GraphicsFunctions.hlsl"


Texture2D<float> gDepthTexture               : register(t0, space0);
Texture2D<float> gLightDepthTexture[1]       : register(t1, space0);
TextureCube<float> gLightDepthCubeTexture[1] : register(t0, space1);
StructuredBuffer<PassCB> gLightsPassCB       : register(t0, space2);
StructuredBuffer<LightCB> gLightCBs          : register(t0, space3);
//  라이트 CB
//  노멀 텍스쳐


SamplerState gLinearWrap : register(s0);
SamplerComparisonState gShadowSampler : register(s1);


cbuffer ShadowMappingCB : register(b0, space0)
{
    float4x4 gCamInvViewProj;
    float gShadowSize;
    uint gLightCount;
    float2 padding;
};
float2 TexOffset(int u, int v)
{
    return float2(u * 1.0f / gShadowSize, v * 1.0f / gShadowSize);
}
float3 TexOffset(int x, int y, int z)
{
    return float3(x * 1.0f / gShadowSize, y * 1.0f / gShadowSize, z * 1.0f / gShadowSize);

}
float PS(QuadPS pin) : SV_TARGET
{
    int DSDIndex = 0;
    int PDIndex = 0;
    // 
    float  depthmap = gDepthTexture.SampleLevel(gLinearWrap, pin.TexC, 0.0f);
    float3 worldPos = CalcWorldPosition(pin.TexC, depthmap, gCamInvViewProj);

    float shadowFactor = 0.0f;
    float shadow;
    float sum = 0;
    float x, y, z;
    float samples = 0.0f;
    const float range = 2.5f;
    for (uint i = 0; i < gLightCount; ++i)
    {

        // directional light
        if (gLightCBs[i].type == LIGHT_TYPE_DIRECTIONAL || gLightCBs[i].type == LIGHT_TYPE_SPOT)
        {
            float4 posFromLight = mul(float4(worldPos, 1.0f), gLightsPassCB[i].ViewProj);
            posFromLight.xyz /= posFromLight.w;

            if (posFromLight.x < -1.0f || posFromLight.x > 1.0f ||
            posFromLight.y < -1.0f || posFromLight.y > 1.0f ||
            posFromLight.z < 0.0f || posFromLight.z > 1.0f)
                return 1.0f;
            posFromLight.x = posFromLight.x / 2.0f + 0.5f;
            posFromLight.y = -posFromLight.y / 2.0f + 0.5f;
            float depth = posFromLight.z - gLightCBs[i].bias;


            for (y = -range; y <= range; y += 1.0)
            {
                for (x = -range; x <= range; x += 1.0)
                {
                    sum += gLightDepthTexture[DSDIndex].SampleCmpLevelZero(gShadowSampler, posFromLight.xy + TexOffset(x, y), depth).r;
                    samples++;
                }
            }
            DSDIndex++;
        }
        // point
        if (gLightCBs[i].type == LIGHT_TYPE_POINT)
        {
            float3 lightPos = worldPos - gLightCBs[i].position;
            float depth = length(lightPos) - gLightCBs[i].bias;

            for (z = -range; z <= range; z += 1.0)
            {
                for (y = -range; y <= range; y += 1.0)
                {
                    for (x = -range; x <= range; x += 1.0)
                    {
                        sum += gLightDepthCubeTexture[PDIndex].SampleCmpLevelZero(gShadowSampler, lightPos + TexOffset(x, y, z), depth).r;
                        samples++;
                    }
                }
            }
            PDIndex++;
        }
    }

    
    // 다이렉션 라이트 그림자계산
   
    shadow = sum / samples;
    shadowFactor += shadow;
    return shadowFactor;
}