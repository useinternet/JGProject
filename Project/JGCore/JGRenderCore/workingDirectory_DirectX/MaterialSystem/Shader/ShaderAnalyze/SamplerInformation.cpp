#include"SamplerInformation.h"
#include<cstring>
using namespace JGRC;
using namespace std;


SamplerInformation::SamplerInformation()
{

}
SamplerInformation::~SamplerInformation()
{

}
void SamplerInformation::AnalyzeSentence(std::string& sentence)
{
	if (!Decryptable(sentence) || StringUtil::FindString(sentence, "#define"))
	{
		return;
	}
	Start();
	// 샘플러가 끝낫다면 바로 멈추고 나간다.
	if (StringUtil::FindString(sentence, hlslType::SAMPLER_End.c_str()))
	{
		m_vSamplerDescs.push_back(m_SamplerDesc);
		m_vModes.push_back(m_Mode);
		Stop();
		return;
	}
	else if (StringUtil::FindString(sentence, hlslType::SAMPLER_DefaultMode_Wrap.c_str()))
	{
		m_Mode = ESamplerMode::Default_Wrap;
		SamplerDefault_WrapMode();
	}
	else if (StringUtil::FindString(sentence, hlslType::SAMPLER_DefaultMode_Clamp.c_str()))
	{
		m_Mode = ESamplerMode::Default_Clamp;
		SamplerDefault_ClampMode();
	}
	else
	{
		m_Mode = ESamplerMode::Custom;
		SamplerCustomMode(sentence);
	}
}
bool SamplerInformation::Decryptable(const std::string& sentence)
{
	if (StringUtil::FindString(sentence, hlslType::SAMPLER_Start.c_str()) || IsProgressing())
	{
		return true;
	}
	return false;
}
void SamplerInformation::WriteShaderData(ofstream& fout)
{
	uint count = 0;
	fout << "@@ SamplerState" << endl;
	fout << "Count : " << m_vSamplerDescs.size() << endl;
	for (auto& iter : m_vSamplerDescs)
	{
		fout << "SamplerMode : " << (int)m_vModes[count] << endl;
		fout << iter.Filter << " " << iter.AddressU << " " << iter.AddressV << " " << iter.AddressW << " "
			 << iter.MipLODBias << " " << iter.MaxAnisotropy << " " << iter.BorderColor[0] << " "
			 << iter.BorderColor[1] << " " << iter.BorderColor[2] << " " << iter.BorderColor[3] << " "
			 << iter.MinLOD << " " << iter.MaxLOD << endl;
		count++;
	}
	fout << "@@" << endl;
}
void SamplerInformation::SamplerCustomMode(string& sentence)
{

	if (StringUtil::FindString(sentence, hlslType::SAMPLER_Filter.c_str()))
	{
		uint ft = ExtractionBracketNumber(sentence);
		m_SamplerDesc.Filter = (D3D11_FILTER)ft;
	}
	else if (StringUtil::FindString(sentence, hlslType::SAMPLER_TextureMode.c_str()))
	{
		uint tm[3];
		ExtractionBracketComaNumber(sentence, tm);
		m_SamplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)tm[0];
		m_SamplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)tm[1];
		m_SamplerDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)tm[2];
	}
	else if (StringUtil::FindString(sentence, hlslType::SAMPLER_MipLODBias.c_str()))
	{
		m_SamplerDesc.MipLODBias = ExtractionBracketFloat(sentence);
	}
	else if (StringUtil::FindString(sentence, hlslType::SAMPLER_MaxAnisotropy.c_str()))
	{
		m_SamplerDesc.MaxAnisotropy = (UINT)ExtractionBracketNumber(sentence);
	}
	else if (StringUtil::FindString(sentence, hlslType::SAMPLER_BoderColor.c_str()))
	{
		ExtractionBracketComaFloat(sentence, m_SamplerDesc.BorderColor);
	}
	else if (StringUtil::FindString(sentence, hlslType::SAMPLER_MinLOD.c_str()))
	{
		m_SamplerDesc.MinLOD = ExtractionBracketFloat(sentence);
	}
	else if (StringUtil::FindString(sentence, hlslType::SAMPLER_MaxLOD.c_str()))
	{
		m_SamplerDesc.MaxLOD = ExtractionBracketFloat(sentence);
	}
}
void SamplerInformation::SamplerDefault_WrapMode()
{
	m_SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	m_SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	m_SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	m_SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	m_SamplerDesc.MipLODBias = 0;
	m_SamplerDesc.MaxAnisotropy = 1;
	m_SamplerDesc.BorderColor[0] = 0; m_SamplerDesc.BorderColor[1] = 0;
	m_SamplerDesc.BorderColor[2] = 0; m_SamplerDesc.BorderColor[3] = 0;
	m_SamplerDesc.MinLOD = 0;
	m_SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
}
void SamplerInformation::SamplerDefault_ClampMode()
{

	m_SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	m_SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_SamplerDesc.MipLODBias = 0;
	m_SamplerDesc.MaxAnisotropy = 1;
	m_SamplerDesc.BorderColor[0] = 0; m_SamplerDesc.BorderColor[1] = 0;
	m_SamplerDesc.BorderColor[2] = 0; m_SamplerDesc.BorderColor[3] = 0;
	m_SamplerDesc.MinLOD = 0;
	m_SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
}