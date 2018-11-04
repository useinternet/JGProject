#include"Common_ps.hlsli"

TEXTURE_START
TEXTURE_CONFIG(0, Default)
Texture2D Default_Texture;
TEXTURE_CONFIG(1, Normal)
Texture2D Normal_Texture;
TEXTURE_END

SAMPLERSTATE_CONFIG_START
SAMPLERSTATE_DEFAULT_WRAPMODE
SamplerState Default_WrapSampler;
SAMPLERSTATE_CONFIG_END


cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
};
float4 main(PixelInputType input) : SV_TARGET
{
    float3 lightDir;
    float lightIntensity;
    float4 color;
	float4 textureColor = Default_Texture.Sample(Default_WrapSampler, input.tex);
	float4 normalMap = Normal_Texture.Sample(Default_WrapSampler, input.tex);
	float3 normalVec;
	// 노멀 맵의 법선 정보를 받아온다.
	normalMap = normalize((normalMap * 2) - 1.0f);

	// 노멀 맵의 법선 정보를 토대로 법선벡터 생성
	normalVec = input.normal + normalMap.x * input.tangent + normalMap.y * input.binormal;
	normalVec = normalize(normalVec);


	//
    color = ambientColor;

    lightDir = -lightDirection;

    lightIntensity = saturate(dot(normalVec,lightDir));
    if (lightIntensity > 0.0f)
    {     
		color += (diffuseColor * lightIntensity);
    }
	color = saturate(color) * textureColor;
	return color;
}