#pragma once
#include"ShaderInformation.h"
#include"MaterialSystem/Data/ShaderData.h"
namespace JGRC
{

	/*
	Exp : hlsl 파일로 부터 샘플러 정보를 해독한다. */
	class CORE_EXPORT SamplerInformation : public ShaderInformation
	{
	private: 
		
		uint m_SamplerCount = 0;                            // 샘플러 번호를 매기기 위한 변수
		std::vector<D3D11_SAMPLER_DESC> m_vSamplerDescs;    // 샘플러 Desc 배열
		std::vector<ESamplerMode>       m_vModes;           // 샘플러 모드 배열
		D3D11_SAMPLER_DESC m_SamplerDesc;                   // 샘플러를 해독하고 저장하기 위한 변수
		ESamplerMode       m_Mode;                          // 샘플러를 해독하고 저장하기 위한 변수
	public:
		SamplerInformation();

		virtual ~SamplerInformation();
		virtual void AnalyzeSentence(std::string& sentence) override;
		virtual bool Decryptable(const std::string& sentence) override;
		virtual void WriteShaderData(std::ofstream& fout) override;
	private:
		void SamplerCustomMode(std::string& sentence);
		void SamplerDefault_WrapMode();
		void SamplerDefault_ClampMode();
	};
}
