#include"GraphicsDefine.hlsl"
#include"GBufferVS.hlsl"



#ifndef __GBUFFER_HLSL__
#define __GBUFFER_HLSL__

GBufferPack PS(VsToPs pin) : SV_Target
{
    GBufferPack gbuffer;


    float3 N = normalize(pin.NormalW);
    float3 T = normalize(pin.TangentW);





 
    ObjectCB object = gObjects[pin.InstanceID];
    MaterialCB material = gMaterials[object.MaterialIndex];
    float4 TextureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);


   // reflect(-toEyeW, pin.NormalW);
    float3 r = reflect(-gToEye, N);
    float4 rcolor = float4(1.0f, 1.0f, 1.0f, 1.0f);

    float4 irradiance = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 prefilter = float4(1.0f, 1.0f, 1.0f, 1.0f);
#ifdef USE_CUBETEXTURE_SLOT0
    //rcolor = gCubeMap[0].SampleLevel(gLinearWrapSampler, r, padding.x);
#endif
#ifdef USE_CUBETEXTURE_SLOT1
    irradiance = gCubeMap[1].SampleLevel(gLinearWrapSampler, r, 0.0f);
#endif
#ifdef USE_CUBETEXTURE_SLOT2
    const float max_reflection_lod = 4.0f;
    prefilter = gCubeMap[2].SampleLevel(gLinearWrapSampler, r, padding.x * max_reflection_lod);
#endif
    // diffuse
#ifdef USE_TEXTURE_SLOT0
    TextureColor = gTexture[0].SampleLevel(gAnisotropicWrapSampler, pin.TexC, padding.x);
#endif
    // normal
#ifdef USE_TEXTURE_SLOT1
    float3 normalMap = gTexture[1].Sample(gAnisotropicWrapSampler, pin.TexC).xyz;
    N = CalcBumpNormal(normalMap, N, T);
#endif




    


    gbuffer.Albedo = prefilter * TextureColor; //float4(material.SurfaceColor, 1.0f) * TextureColor + irradiance;
    gbuffer.Normal = float4((N + 1.0f) * 0.5f, 1.0f);
    gbuffer.Specular = float4(0.0f, 0.0f, 1.0f, 1.0f);
    gbuffer.Depth = pin.Depth;

    return gbuffer;
}

#endif