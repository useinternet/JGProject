


cbuffer Material
{
	float3 Ambient;           // 기본 1
	float  ReflectIntensity;  // 기본 0
	float3 SpecularColor;     // 기본 1
	float  SpecularPower;     // 기본 1
	float3 Emissive;          // 기본 1
	float  SpecularIntensity;         // 기본 0
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
	float4 worldPos  : TEXCOORD1;
	float4 Pos : TEXCOORD2;
	float3 normal : NORMAL;
};
struct PixelOutputType
{
	float4 Pos_Depth     : SV_TARGET0;
	float4 Normal_SpecPw : SV_TARGET1;
	float4 Albedo_SpecIts : SV_TARGET2;
	float4 SpecColor_Pad : SV_TARGET3;
	float4 DepthTarget   : SV_TARGET4;
};
PixelOutputType main(PixelInputType input) : SV_TARGET
{
	PixelOutputType output;
//////////////////////////////////////////////////////////////////
///////////////////////  넣어야할 값 선언  ////////////////////////
//////////////////////////////////////////////////////////////////
float3 st_WorldPos;   float  st_Depth;
float3 st_Normal;     float  st_SpecPw;
float3 st_Albedo;     float  st_SpecIntensity;
float4 st_SpecColor;
//////////////////////////////////////////////////////////////////
//////////////////////////  계산  ////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
////////////////////////   최종 값 대입   /////////////////////////
//////////////////////////////////////////////////////////////////
	// Pos_Depth
	st_WorldPos = input.worldPos.xyz;
	st_Depth    = input.worldPos.z / input.worldPos.w;
	// Normal_SpecPw
	st_Normal = input.normal;
	st_SpecPw = SpecularPower;
	// Albedo_Custom
	st_Albedo = Ambient;
	st_SpecIntensity = SpecularIntensity;
	// SpecColor_Pad
	st_SpecColor = float4(SpecularColor, 1.0f);
//////////////////////////////////////////////////////////////////
////////////////// 렌더타겟에 정보 저장 ////////////////////////////
//////////////////////////////////////////////////////////////////
	output.Pos_Depth     = float4(st_WorldPos, st_Depth);
	output.Normal_SpecPw = float4(st_Normal, st_SpecPw);
	output.Albedo_SpecIts = float4(st_Albedo, st_SpecIntensity);
	output.SpecColor_Pad = st_SpecColor;
	output.DepthTarget = float4(st_Depth, st_Depth, st_Depth, 1.0f);
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
	return output;
}