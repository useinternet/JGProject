#define MAX_POINTLIGHT 20
#define MAX_SPOTLIGHT 10

Texture2D T_Pos_Depth      : register(t0);
Texture2D T_Normal_SpecPow : register(t1);
Texture2D T_Albedo         : register(t2);
Texture2D T_Specular       : register(t3);

SamplerState ClampSampler;

struct DirectionLight
{
	float3 AmbientUp;   // padding1
	float3 AmbientDown; // padding2
	float3 Direction;   // padding3
	float3 Color;       // padding4
};
struct PointLight
{
	float3 Position;
	float  RangeRcp;
	float3 Color;  // padding
};
struct SpotLight
{
	float3 Position;
	float  RangeRcp;
	float3 Direction;
	float  CosOuterCone;
	float3 Color;
	float  CosInnerCone;
};
struct Material
{
	float3 position;
	float3 normal;
	float  depth;       
	float3 specColor;    
	float  specPower;             
	float  specInstensity;        
};
Material InitMat(float3 WorldPos, float3 Normal, float Depth,
	float3 specColor, float specPw, float specIns)
{
	Material mat;
	mat.position = WorldPos;
	mat.normal = Normal;
	mat.depth = Depth;
	mat.specColor = specColor;
	mat.specPower = specPw;
	mat.specInstensity = specIns;
	return mat;
}
cbuffer LightBuffer
{
	DirectionLight DirLight;
	PointLight     PntLight[MAX_POINTLIGHT];
	SpotLight      SptLight[MAX_SPOTLIGHT];
	int DirectionLightCount;
	int PointLightCount;
	int SpotLightCount;
};
float3 CalcAmbient(Material mat)
{
	float  up = mat.normal.y * 0.5f + 0.5f;
	float3 ambient = DirLight.AmbientDown + up * DirLight.AmbientUp;
	return ambient;
}
float3 CalcDirectional(Material mat, float3 CameraPos)
{
	float NDotL = dot(-DirLight.Direction, mat.normal);
	float3 color = saturate(NDotL) * DirLight.Color;


	float3 ToEye = CameraPos - mat.position;
	ToEye = normalize(ToEye);
	float3 HalfWay = normalize(ToEye + DirLight.Direction);
	float NDotH = saturate(dot(HalfWay, mat.normal));
	color += DirLight.Color * pow(NDotH, mat.specPower) * mat.specInstensity;

	return saturate(color);
}
float3 CalcPoint(Material mat,float3 CameraPos,PointLight pntLight)
{
	float3 ToLight = pntLight.Position - mat.position;
	float3 ToEye = CameraPos - mat.position;
	float DistToLight = length(ToLight);

	// diffuse
	ToLight /= DistToLight;
	float NDotL = saturate(dot(ToLight, mat.normal));
	float3 color = pntLight.Color * NDotL;

	// specular
	ToEye = normalize(ToEye);
	float3 HalfWay = normalize(ToEye + ToLight);
	float NDotH = saturate(dot(HalfWay, mat.normal));
	color += pntLight.Color * pow(NDotH, mat.specPower)* mat.specInstensity;

	// att
	float DistToLightNorm = 1.0 - saturate(DistToLight * pntLight.RangeRcp);
	float Attn = DistToLightNorm * DistToLightNorm;
	color *= Attn;
	return color;
}
float3 CalcSpot(Material mat, float3 CameraPos,SpotLight sptLight)
{
	float3 ToLight = sptLight.Position - mat.position;
	float3 ToEye = CameraPos - mat.position;
	float DistToLight = length(ToLight);

	// diffuse
	ToLight /= DistToLight;
	float NDotL = saturate(dot(ToLight, mat.normal));
	float3 color = sptLight.Color * NDotL;

	// specular
	ToEye = normalize(ToEye);
	float3 HalfWay = normalize(ToEye + ToLight);
	float NDotH = saturate(dot(HalfWay, mat.normal));
	color += sptLight.Color * pow(NDotH, mat.specPower)* mat.specInstensity;

	// cone att
	float cosAng = dot(-sptLight.Direction, ToLight);
	float conAtt = saturate((cosAng - sptLight.CosOuterCone) * sptLight.CosInnerCone);
	conAtt *= conAtt;

	float DistToLightNorm = 1.0 - saturate(DistToLight * sptLight.RangeRcp);
	float Attn = DistToLightNorm * DistToLightNorm;
	color *= (Attn * conAtt);

	return color;
}
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 CameraDir : TEXCOORD1;
};
float4 main(PixelInputType input) : SV_TARGET
{
	float3  ShadowMap     = T_ShadowMap.Sample(ClampSampler,input.tex).rgb;
	float4 Pos_Depth      = T_Pos_Depth.Sample(ClampSampler, input.tex);
	float4 Normal_SpecPow = T_Normal_SpecPow.Sample(ClampSampler, input.tex);
	float4 Albedo_SpecInt = T_Albedo.Sample(ClampSampler, input.tex);
	// °ª ÃßÃâ
	float3 Albedo        = saturate(Albedo_SpecInt.xyz);
	float3 SpecularColor = saturate(T_Specular.Sample(ClampSampler, input.tex).xyz);
	float3 WorldPos      = Pos_Depth.xyz;
	float  Depth         = Pos_Depth.w;
	float3 Normal        = normalize(Normal_SpecPow.xyz);
	float  SpecPow       = Normal_SpecPow.w;
	float  SpecularIntensity = Albedo_SpecInt.w;

	Material mat = InitMat(WorldPos, Normal, Depth, SpecularColor, SpecPow,
		SpecularIntensity);
	float3 finalColor = float3(0.0f,0.0f,0.0f);
	if (DirectionLightCount == 1)
	{
		finalColor = CalcAmbient(mat);
		finalColor += CalcDirectional(mat, input.CameraDir);
	}
	for (int i = 0; i < PointLightCount; ++i)
	{
		finalColor += CalcPoint(mat, input.CameraDir, PntLight[i]);
	}
	for (int j = 0; j < SpotLightCount; ++j)
	{
		finalColor += CalcSpot(mat, input.CameraDir, SptLight[j]);
	}
	finalColor *= Albedo;
	finalColor = finalColor;
	return float4(Depth, Depth, Depth, 1.0f);
}