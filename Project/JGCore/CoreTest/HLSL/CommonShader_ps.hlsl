#include"Common_ps.hlsli"
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
}
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};
float4 main(PixelInputType input) : SV_TARGET
{
    float3 lightDir;
    float lightIntensity;
    float4 color;

    color = ambientColor;

    lightDir = -lightDirection;

    lightIntensity = saturate(dot(input.normal,lightDir));
    if (lightIntensity > 0.0f)
    {     
		color += (diffuseColor * lightIntensity);
    }
    color = saturate(color);

	return color;
	//return float4(Sample, Sample, Sample, Sample);
}