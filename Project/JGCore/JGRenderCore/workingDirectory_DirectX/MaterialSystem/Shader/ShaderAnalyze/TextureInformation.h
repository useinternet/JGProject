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
		virtual void WriteShaderData(std::ofstream& fout) override;
	private:
		void ExtractionTextureName(std::string& s);
	};
}
