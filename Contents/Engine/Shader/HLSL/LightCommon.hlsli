
Texture2D DepthTexture        : register(t0);
Texture2D ColorSpecIntTexture : register(t1);
Texture2D NormalTexture       : register(t2);
Texture2D SpecPowTexture      : register(t3);
#define MAX_DIRECTION_LIGHT 1
#define MAX_POINT_LIGHT     50
#define MAX_SPOT_LIGHT      50



struct SCENE_DATA
{
	float  LinearDepth;
	float3 Color;
	float3 Normal;
	float  SpecInt;
	float  SpecPower;
};
struct AmbientLight
{

};
struct DirectionLight
{

};
struct PointLight
{

};
struct SpotLight
{

};
cbuffer UnpackBuffer : register (b0)
{
	float4 PerspectiveValues : packoffset(c0);
	matrix ViewInv : packoffset(c1);
}
#define EyePosition (ViewInv[3].xyz)





float ConvertDepthToLinear(float depth)
{
	float linearDepth = PerspectiveValues.z / (depth + PerspectiveValues.w);
	return linearDepth;
}
SCENE_DATA UnpackData(int2 location)
{
	SCENE_DATA Out;
	int3 location3 = int3(location, 0);

	float depth = DepthTexture.Load(location3).x;

	Out.LinearDepth = ConvertDepthToLinear(depth);


	float4 baseColorSpecInt = ColorSpecIntTexture.Load(location3);
	Out.Color = baseColorSpecInt.xyz;
	Out.SpecInt = baseColorSpecInt.w;
	Out.Normal = NormalTexture.Load(location3).xyz;
	Out.Normal = normalize(Out.Normal * 2.0 - 1.0);
	Out.SpecPower = SpecPowTexture.Load(location3).x;
	Out.SpecPower = g_SpecPowerRange.x + g_SpecPowerRange.y * Out.SpecPower;
	return Out;
}

float3 CalcWorldPos(float2 csPos, float linearDepth)
{
	float4 pos;
	pos.xy = csPos.xy * PerspectiveValues.xy * linearDepth;
	pos.z = linearDepth;
	pos.w = 1.0f;
	return mul(pos, ViewInv).xyz;
}