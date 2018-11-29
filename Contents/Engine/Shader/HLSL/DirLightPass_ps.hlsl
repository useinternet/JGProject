#include"Common.hlsli"
#include"LightCommon.hlsli"

static const float3 AmbientDown = float3(0.1f, 0.1f, 0.15f); 
static const float3 AmbientRange =  float3(0.2f, 0.2f, 0.25f); 
static const float3 DirToLight    = float3(1.0f, 1.0f, 0.0f);
static const float3 DirLightColor = float3(0.7f, 0.7f, 0.7f);




float3 CalcAmbient(float3 normal, float3 color)
{
	// Convert from [-1, 1] to [0, 1]
	float up = normal.y * 0.5 + 0.5;

	// Calculate the ambient value
	float3 ambient = AmbientDown + up * AmbientRange;

	// Apply the ambient value to the color
	return ambient * color;
}

float3 CalcDirectional(float3 position, SCENE_DATA data)
{
	// Phong diffuse
	float NDotL = dot(DirToLight, data.Normal);
	float3 finalColor = DirLightColor.rgb * saturate(NDotL);

	// Blinn specular
	float3 ToEye = EyePosition - position;
	ToEye = normalize(ToEye);
	float3 HalfWay = normalize(ToEye + DirToLight);
	float NDotH = saturate(dot(HalfWay, data.Normal));
	finalColor += DirLightColor.rgb * pow(NDotH, data.SpecPower) * data.SpecInt;

	return finalColor * data.Color.rgb;
}

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 cpPos	: TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
	SCENE_DATA data = UnpackData(input.position.xy);
    float3 worldPos = CalcWorldPos(input.cpPos, data.LinearDepth);

	float3 Color = CalcAmbient(data.Normal, data.Color);
	Color += CalcDirectional(worldPos, data);

	return float4(Color, 1.0f);
}
	