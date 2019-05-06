#include"SkyBoxVS.hlsl"



GBufferPack PS(VertexOut pin) : SV_Target
{
    GBufferPack gbuffer;
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
#ifdef USE_CUBETEXTURE_SLOT0
    color = gCubeMap[0].Sample(gAnisotropicWrapSampler, pin.PosL);
#endif

    gbuffer.Albedo = float4(color.xyz * 1.2f, 0.0f);
    gbuffer.Normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    gbuffer.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    gbuffer.Depth = 1.0f;
    return gbuffer;
}