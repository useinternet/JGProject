


Texture2D Default_Texture;
Texture2D Normal_Texture;
SamplerState Default_WrapSampler;

cbuffer Material
{
	float3 Ambient;           // 기본 1
	float  ReflectIntensity;  // 기본 0
	float3 SpecularColor;     // 기본 1
	float  SpecularPower;     // 기본 1
	float3 Emissive;          // 기본 1
	float  CustomVar;         // 기본 0
};
/* 렌더링 순서도
1. 기본 머터리얼로 반사 맵핑을 실행하는지 안하는지 검사( reflection = true, false )
 case 1 : 반사 맵핑이라면
    R1. view matrix를 미러링 시켜 텍스쳐를 굽는다.( 한번더 화면 렌더링 <- 대신 컬링 처리)
	    (reflectionColor에 * reflectionIntensity) 에 기본 텍스쳐혹은 Ambient를 곱해서 최종 색깔 반환
	R2. 해당 머터리얼의 Albedo기본값은 위에 반사 셰이더에서 구운 텍스쳐가 된다.
	R3. 기본 렌더링 수행
case 2 : 반사 맵핑이 아니라면
    1. 기본 렌더링 수행 <- 현재 작업중
*/
// 정반사광 세기 기본 = 1
//specularColor = 1,1,1,1
/* 렌더 타겟 네개
   1      2      3     4
  |    worldPos     | Depth |   // 월드 좌표, 김피
  |    normal       |SpecPw |   // 노멀 좌표, 정반사광 세기
  |    albedo       | pad   |   // 기본 반사색, 반사 세기
  | specularColor   | pad   |   // 정반사광 색 / 
*/
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
	float4 textureColor = Default_Texture.Sample(Default_WrapSampler, input.tex);
	float4 normalMap    = Normal_Texture.Sample(Default_WrapSampler, input.tex);
	float3 normalVec;
	//// 노멀 맵의 법선 정보를 받아온다.
	normalMap = normalize((normalMap * 2) - 1.0f);

	// 노멀 맵의 법선 정보를 토대로 법선벡터 생성
	normalVec = input.normal + normalMap.x * input.tangent + normalMap.y * input.binormal;
	normalVec = normalize(normalVec);

	textureColor *= float4(normalVec,1.0f);


	return textureColor;
}