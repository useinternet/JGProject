#pragma once
#include"ShaderInformation.h"

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
		std::vector<uint>       m_vStride;
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
		virtual void WriteShaderData(std::ofstream& fout) override;
	};

}
