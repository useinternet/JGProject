#include"ShaderAnalyzer.h"
#include"LayoutInformation.h"
#include"CBufferInformation.h"
#include"SamplerInformation.h"
#include"TextureInformation.h"
using namespace JGRC;
using namespace std;




ShaderAnalyzer::ShaderAnalyzer()
{

	m_InputLayoutInfor = make_unique<LayoutInformation>();
	m_CBufferInfor     = make_unique<CBufferInformation>();
	m_SamplerInfor     = make_unique<SamplerInformation>();
	m_TextureInfor     = make_unique<TextureInformation>();
}
ShaderAnalyzer::~ShaderAnalyzer()
{


}
bool ShaderAnalyzer::Analyze(const string& hlslPath, const EShaderType ShaderType)
{
	fstream fin;
	string buffer; // 입력 받아올 문자열
	string path;   // 패쓰

	fin.open(hlslPath.c_str());
	if (!fin.is_open())
	{
		return false;
	}
	while (!fin.eof())
	{
		getline(fin, buffer);
		if (!RemoveRemark(buffer))
		{
			continue;
		}

		// 인크루드한 헤더 hlsl파일을 재귀 탐색한다.
		if (StringUtil::FindString(buffer, hlslType::INCLUDE.c_str() ))
		{
			path = IncludeAyz(hlslPath,buffer);
			Analyze(path, ShaderType);
		}
		// 입력 레이아웃 정보 저장
		else if (StringUtil::FindString(buffer, hlslType::INPUTLAYOUT.c_str()) || m_InputLayoutInfor->IsProgressing())
		{
			m_InputLayoutInfor->AnalyzeSentence(buffer);
		}
		// 상수 버퍼 저장
		else if (StringUtil::FindString(buffer, hlslType::CBUFFER.c_str()) || m_CBufferInfor->IsProgressing())
		{
			m_CBufferInfor->AnalyzeSentence(buffer);
		}
		else if (StringUtil::FindString(buffer, hlslType::SAMPLER_Start.c_str()) || m_SamplerInfor->IsProgressing())
		{
			m_SamplerInfor->AnalyzeSentence(buffer);
		}
		else if (StringUtil::FindString(buffer, hlslType::TEXTURE_Start.c_str()) || m_TextureInfor->IsProgressing())
		{
			m_TextureInfor->AnalyzeSentence(buffer);
		}
	}
	fin.close();
	return true;
}
bool ShaderAnalyzer::RemoveRemark(std::string& sentence)
{
	if (StringUtil::FindString(sentence, "//"))
	{
		uint pos = sentence.find("//");
		if (pos < 1)
		{
			return false;
		}
		StringUtil::DelString(sentence, sentence.substr(pos, sentence.length() - 1).c_str());
	}
	if (StringUtil::FindString(sentence, "*/"))
	{
		uint pos = sentence.find("*/");
		m_bIgnore = false;
		if (pos < 1)
		{
			return false;
		}
		StringUtil::DelString(sentence, sentence.substr(0, pos + 2).c_str());
		return true;
	}
	else if (StringUtil::FindString(sentence, "/*"))
	{
		uint pos = sentence.find("/*");
		m_bIgnore = true;
		if (pos < 1)
		{
			return false;
		}
		StringUtil::DelString(sentence, sentence.substr(pos, sentence.length() - 1).c_str());
		return true;
	}
	if (m_bIgnore)
	{
		return false;
	}
	return true;
}
string ShaderAnalyzer::IncludeAyz(string hlslPath, string& sentence)
{
	// 인크루드한 path만 따로 떼놓는다.
	StringUtil::DelString(sentence, "#include");
	StringUtil::DelChar(sentence, '\"');

	StringUtil::Path_DelEndPath(hlslPath);
	return hlslPath + sentence;
}