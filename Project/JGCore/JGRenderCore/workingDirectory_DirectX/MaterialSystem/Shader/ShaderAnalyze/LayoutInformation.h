#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"

// 버텍스 레이아웃은 #define으로 설정
// 레이아웃 정보
// 첫번째 이름 으로 저장
// 시멘틱 번호 저장
// 포맷 형식 -> float , float2, float3, float4  이렇게 4개
// 입력 슬롯 <- 이거는 메타 데이터로 번역해서 저장..
// 오프셋은 순차적으로
// 버텍스 형식인지
// 인스턴스 0,


namespace JGRC
{
	class CORE_EXPORT Layout
	{
	public:
		std::string sementicName;
		uint sementicNum;
		DXGI_FORMAT format;
		uint inputSlot;
		uint offset;
		D3D11_INPUT_CLASSIFICATION inputClass;
		uint InstanceDataStepRate;
	};
	class CORE_EXPORT LayoutInformation
	{
	private:
		static const std::string INPUTLAYOUT;
		static const std::string INPUTSLOT;
		static const std::string INSTNACE;
		static const std::string FLOAT;
		static const std::string FLOAT2;
		static const std::string FLOAT3;
		static const std::string FLOAT4;
	private:
		std::vector<Layout> m_vLayout;
		bool   m_bIsInstance = false;
		bool   m_bIsProgressing = false;
		uint   inputSlot = 0;
	public:
		LayoutInformation();
		~LayoutInformation();
		bool IsProgressing() const;
		void AnalyzeSentence(std::string& sentence);
	private:
		uint ExtractionBracketNumber(const std::string& s);
	};

}
