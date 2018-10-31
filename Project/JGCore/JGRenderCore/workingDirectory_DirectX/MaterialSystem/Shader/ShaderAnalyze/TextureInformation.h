#pragma once
#include"ShaderInformation.h"


namespace JGRC
{
	class CORE_EXPORT TextureInformation : public ShaderInformation
	{
	private:
		std::map<uint, std::string> m_mInformation;
	public:
		TextureInformation();
		virtual ~TextureInformation();

		virtual void AnalyzeSentence(std::string& sentence) override;
		virtual bool Decryptable(const std::string& sentence) override;
		const std::string& GetTextureName(const uint idx) const;
		uint Size() const;
	private:
		void ExtractionTextureName(std::string& s);
	};
}
