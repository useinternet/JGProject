#include"GraphicsDefine.hlsl"
#include"QuadVS.hlsl"

#ifndef __SCENE_HLSL__
#define __SCENE_HLSL__





static const float3 SunDir = { 0.0f, 0.0f, 1.0f };
float4 PS(VsToPs pin) : SV_Target
{


    SceneData data;
    data = GetSceneData(pin.TexC);
#ifdef SCENE_DEBUG_MODE_ALBEDO
    return float4(data.Albedo, 1.0f);
#endif
#ifdef SCENE_DEBUG_MODE_NORMAL
    return float4(normalize(data.Normal), 1.0f);
#endif
#ifdef SCENE_DEBUG_MODE_SPECULAR
    return float4(data.F0, 1.0f);
#endif
#ifdef SCENE_DEBUG_MODE_DEPTH
    return float4(data.Depth,data.Depth,data.Depth, 1.0f);
#endif

    float3 diffuseColor = data.Albedo;




    // 큐브 맵이면 그냥 렌더링
    if (data.IsBackGround)
        return float4(diffuseColor.xyz, 1.0f);


    float3 ambient = float3(0.25f, 0.25f, 0.25f); //* color;
  

    // 필요 상수들 
    float3 N = normalize(data.Normal);
    float3 L = normalize(-SunDir);
    float3 V = normalize(gToEye - data.WorldPosition);
    float3 H = GetHalfVector(L, V);
    float NDotL = max(dot(N, L), 0.0f);
    float NDotV = max(dot(N, V), 0.0f);
    float NDotH = max(dot(N, H), 0.0f);
    float VDotH = max(dot(V, H), 0.0f);
    float VDotL = max(dot(V, L), 0.0f);



    float roughtness = clamp(padding.x, 0.01f, 1.0f);
    float metalic = clamp(padding.y, 0.01f, 1.0f);


    float3 F0 = lerp(float3(0.03, 0.03, 0.03), diffuseColor.rgb, metalic);
    float3 specular = Specular(NDotL, NDotH, NDotV, F0, roughtness);


    float3 color = diffuseColor - diffuseColor * metalic;
    color = Diffuse(color);






    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    Lo = NDotL * (color * (float3(1.0f, 1.0f, 1.0f) - specular) + specular);



    ambient *= diffuseColor.rgb;
    color = ambient + Lo;

    //
    return float4(diffuseColor, 1.0f);
}

#endif