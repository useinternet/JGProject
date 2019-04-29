#include"GraphicsDefine.hlsl"

#define SCENE_ALBEDO 0
#define SCENE_NORMAL 1
#define SCENE_SPECULAR 2
#define SCENE_DEPTH 3


static const float2 gTexCoords[6] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};
 
struct VsToPs
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD0;
};

VsToPs VS(uint vid : SV_VertexID)
{
    VsToPs vout;
    
    vout.TexC = gTexCoords[vid];
    vout.PosH = float4(2.0f * vout.TexC.x - 1.0f, 1.0f - 2.0f * vout.TexC.y, 0.0f, 1.0f);
    return vout;
}

static const float3 SunDir = { 0.0f, 0.0f, 1.0f };
float4 PS(VsToPs pin) : SV_Target
{

#ifdef SCENE_DEBUG_MODE_ALBEDO
    return float4(gTexture[SCENE_ALBEDO].Sample(gAnisotropicWrapSampler, pin.TexC).xyz, 1.0f);
#endif
#ifdef SCENE_DEBUG_MODE_NORMAL
    return float4(gTexture[SCENE_NORMAL].Sample(gAnisotropicWrapSampler, pin.TexC).xyz, 1.0f);
#endif
#ifdef SCENE_DEBUG_MODE_SPECULAR
    return float4(gTexture[SCENE_SPECULAR].Sample(gAnisotropicWrapSampler, pin.TexC).xyz, 1.0f);
#endif
#ifdef SCENE_DEBUG_MODE_DEPTH
    float debug_depth = gTexture[SCENE_DEPTH].Sample(gAnisotropicWrapSampler, pin.TexC).r;
    debug_depth = ConvertLinearDepth(debug_depth);
    return float4(debug_depth,debug_depth,debug_depth, 1.0f);
#endif

    float4 diffuseColor = gTexture[SCENE_ALBEDO].Sample(gAnisotropicWrapSampler, pin.TexC);



    // Å¥ºê ¸ÊÀÌ¸é ±×³É ·»´õ¸µ
    if (diffuseColor.a == 0.0f)
        return float4(diffuseColor.xyz, 1.0f);

    float3 worldpos = CalcWorldPosition(pin.TexC, SCENE_DEPTH);
    float  depth    = ConvertLinearDepth(gTexture[SCENE_DEPTH].Sample(gPointWrapSampler, pin.TexC).r);
    float3 normal   = gTexture[SCENE_NORMAL].Sample(gAnisotropicWrapSampler, pin.TexC).xyz;
    normal = normalize(normal);
    normal = (normal * 2.0f) - 1.0f;


    float3 color = diffuseColor.xyz;
    float3 ambient = float3(0.25f, 0.25f, 0.25f) * color;
  

    float3 N = normal;
    float3 L = normalize(-SunDir);
    float NDotL = dot(N, L);


    color = saturate(color * NDotL);
    color = ambient + color;
    return float4(color, 1.0f);

}