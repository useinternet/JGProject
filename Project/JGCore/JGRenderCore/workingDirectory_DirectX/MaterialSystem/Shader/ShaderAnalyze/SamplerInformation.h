#pragma once
#include"ShaderInformation.h"
namespace JGRC
{
	enum class CORE_EXPORT ESamplerMode
	{
		Default_Wrap,
		Default_Clamp,
		Custom
	};
	class CORE_EXPORT SamplerInformation : public ShaderInformation
	{
	private:
		uint m_SamplerCount = 0;
		std::vector<D3D11_SAMPLER_DESC> m_vSamplerDescs;
		std::vector<ESamplerMode>       m_vModes;
		D3D11_SAMPLER_DESC m_SamplerDesc;
		ESamplerMode       m_Mode;
	public:
		SamplerInformation();

		virtual ~SamplerInformation();
		virtual void AnalyzeSentence(std::string& sentence) override;
		D3D11_SAMPLER_DESC GetDesc(const uint idx) const;
		ESamplerMode GetModeType(const uint idx) const;
		uint Size() const;
	private:
		void SamplerCustomMode(std::string& sentence);
		void SamplerDefault_WrapMode();
		void SamplerDefault_ClampMode();
	};
}
