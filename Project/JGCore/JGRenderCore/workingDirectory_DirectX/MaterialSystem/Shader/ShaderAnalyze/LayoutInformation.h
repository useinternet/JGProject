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
	/*
	Exp : hlsl 파일을 해독하여 입력 레이아웃 정보를 가져온다. */
	class CORE_EXPORT LayoutInformation : public ShaderInformation
	{
		class LayoutDesc
		{
		public:
			std::string SemanticName;
			UINT SemanticIndex;
			DXGI_FORMAT Format;
			UINT InputSlot;
			UINT AlignedByteOffset;
			D3D11_INPUT_CLASSIFICATION InputSlotClass;
			UINT InstanceDataStepRate;
		};
	private:
		std::vector<LayoutDesc> m_vLayout;  // 레이아웃 목록 배열
		bool   m_bIsInstance = false;                     // 인스턴스 변수를 저장하는지 여부 
		uint   m_InstanceDataSR = 1;                      // 인스턴스 DataSetpRate 를 저장하기 위한 변수
		//
		uint   m_inputSlot = 0;                           //  현재 입력 슬롯
		uint   m_pvInputSlot = 0;                         //  전 입력 슬롯
		//
		uint   m_accOffset = 0;                           // 누적 오프셋
	public:
		LayoutInformation();
		virtual ~LayoutInformation();
		virtual void AnalyzeSentence(std::string& sentence) override;
		virtual bool Decryptable(const std::string& sentence) override;

		void MakeInputLayoutArray(class InputLayout* layout);
		/*
		Exp : 목록 사이즈를 가져온다. */
		uint Size();
	};

}
