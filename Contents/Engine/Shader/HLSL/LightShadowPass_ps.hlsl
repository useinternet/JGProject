
Texture2D T_Pos_Depth : register(t0);
Texture2D T_ShadowTexture : register(t1);
SamplerState ClampSampler : register(s0);
SamplerComparisonState SamplerShadow : register(s1);
cbuffer LightMatrixBuffer
{
	matrix LightViewProjMatrix;
	matrix InvCameraMatrix;
	matrix InvProjMatrix;

};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex      : TEXCOORD0;
};
float4 main(PixelInputType input) : SV_TARGET
{
	float  color = 0.0f;     // 최종 컬러
	float  Biaos = 0.001f; // 바이어스(부동소수점 오차)
	float3 WorldPos = T_Pos_Depth.Sample(ClampSampler, input.tex).xyz; // 월드 좌표 텍스쳐

	// 광원 관점 위치 파악
	float4 position = float4(WorldPos, 1.0f);

	//position = mul(position, InvProjMatrix);

	position = mul(position, LightViewProjMatrix);
	position = mul(position, InvCameraMatrix);
	// 투영 텍스쳐
	float2 projTex;
	projTex = position.xy / position.w;
	projTex.y = -projTex.y;
	projTex = 0.5f * projTex + 0.5f;

	//
	float LightDepth;   // 광원 관점에서 뽑을 깊이 값
	float Depth;        // 카메라 관점에서 만들어둔 깊이 버퍼
	if (saturate(projTex.x) == projTex.x && saturate(projTex.y) == projTex.y)
	{
		Depth = T_ShadowTexture.Sample(ClampSampler, projTex).w; // 깊이 텍스쳐 샘플링( 광원관점 투영 샘플링)
		LightDepth = (position.z / position.w) - Biaos;      // 

		//color = T_ShadowTexture.SampleCmpLevelZero(SamplerShadow, projTex, LightDepth).r;
		if (LightDepth < Depth) //
		{
			color = 1.0f;
		}
	}
	//else
	//{
	//	color = 1.0f;
	//}
	return  float4(LightDepth, LightDepth, LightDepth, 1.0f);
}