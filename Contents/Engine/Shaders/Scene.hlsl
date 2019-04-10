

Texture2D gAlbedoTexture : register(t0);
Texture2D gNormalTexture : register(t1);
Texture2D gSpecularTexture : register(t2);
Texture2D gDepthTexture : register(t3);

SamplerState AnisotropicWrap : register(s0);

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
    return float4(gAlbedoTexture.Sample(AnisotropicWrap, pin.TexC).xyz, 1.0f);
#endif
#ifdef SCENE_DEBUG_MODE_NORMAL
    return float4(gNormalTexture.Sample(AnisotropicWrap, pin.TexC).xyz, 1.0f);
#endif
#ifdef SCENE_DEBUG_MODE_SPECULAR
    return float4(gSpecularTexture.Sample(AnisotropicWrap, pin.TexC).xyz, 1.0f);
#endif
#ifdef SCENE_DEBUG_MODE_DEPTH
    float depth = gDepthTexture.Sample(AnisotropicWrap, pin.TexC).r;
    return float4(depth,depth,depth, 1.0f);
#endif
   
    float4 diffuseColor = gAlbedoTexture.Sample(AnisotropicWrap, pin.TexC);

    // Å¥ºê ¸ÊÀÌ¸é ±×³É ·»´õ¸µ
    if (diffuseColor.a == 0.0f)
        return float4(diffuseColor.xyz, 1.0f);

    float3 N = gNormalTexture.Sample(AnisotropicWrap, pin.TexC).xyz;
    N = normalize(N);
    N = (N * 2) - 1.0f;

    float3 L = normalize(-SunDir);
    float NDotL = dot(N, L);

    return diffuseColor * NDotL;
}