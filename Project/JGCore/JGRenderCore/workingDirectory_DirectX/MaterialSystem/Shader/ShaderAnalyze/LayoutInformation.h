#pragma once
#include"ShaderInformation.h"

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

	class CORE_EXPORT LayoutInformation : public ShaderInformation
	{
	private:
		class Layout
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
	private:
		std::vector<D3D11_INPUT_ELEMENT_DESC> m_vLayout;
		bool   m_bIsInstance = false;
		uint   m_InstanceDataSR = 1;
		//
		uint   m_inputSlot = 0;
		uint   m_pvInputSlot = 0;
		//
		uint   m_accOffset = 0;
	public:
		LayoutInformation();
		virtual ~LayoutInformation();
		virtual void AnalyzeSentence(std::string& sentence) override;
		D3D11_INPUT_ELEMENT_DESC& GetDesc(const uint idx);
		D3D11_INPUT_ELEMENT_DESC* GetDescAddress();
		uint Size();
	};

}
