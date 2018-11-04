#include"Common_ps.hlsli"
cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float  specularPower;
	float4 specularColor;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDir : TEXCOORD1;
};
float4 main(PixelInputType input) : SV_TARGET
{
    float3 lightDir;
    float lightIntensity;
    float4 color;
	float4 specular;
	float3 reflection;
    color = ambientColor;


	specular = float4(0, 0, 0, 0);

    lightDir = -lightDirection;

    lightIntensity = saturate(dot(input.normal,lightDir));
    if (lightIntensity > 0.0f)
    {     
		color += (diffuseColor * lightIntensity);
		color = saturate(color);
		reflection = normalize(2 * lightIntensity * input.normal - lightDir);
		specular = pow(saturate(dot(reflection, input.viewDir)), specularPower);
    }
	color = saturate(color + specular);
	return color;
}