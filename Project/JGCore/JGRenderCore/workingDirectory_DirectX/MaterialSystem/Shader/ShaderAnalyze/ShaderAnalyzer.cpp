#include"ShaderAnalyzer.h"
#include"LayoutInformation.h"
#include"CBufferInformation.h"
#include"SamplerInformation.h"
#include"TextureInformation.h"
using namespace JGRC;
using namespace std;
ShaderAnalyzer::AyzInformation::AyzInformation()
{
	InputLayout = make_unique<LayoutInformation>();
	CBuffer = make_unique<CBufferInformation>();
	SamplerState = make_unique<SamplerInformation>();
	Texture = make_unique<TextureInformation>();
}
bool ShaderAnalyzer::Analyze(const std::string& hlslPath, const EShaderType ShaderType)
{
	AyzInformation infor;
	infor.hlslPath = hlslPath;
	if (ReadShader(hlslPath, &infor))
	{
		m_mInformation.insert(pair<EShaderType, AyzInformation>(ShaderType, move(infor)));
		return true;
	}
	return false;
}
bool ShaderAnalyzer::ReadShader(const std::string& hlslPath, AyzInformation* infor)
{
	fstream fin;
	string buffer; // 입력 받아올 문자열
	string path;   // 패쓰
				   // 파일 열기
	fin.open(hlslPath.c_str());
	if (!fin.is_open())
	{
		JGLOG(log_Error, "JGRC::ShaderAnalyzer::Analyze", hlslPath);
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
		if (StringUtil::FindString(buffer, hlslType::INCLUDE.c_str()))
		{
			path = IncludeAyz(hlslPath, buffer);
			ReadShader(path, infor);
		}
		infor->InputLayout->AnalyzeSentence(buffer);
		infor->CBuffer->AnalyzeSentence(buffer);
		infor->SamplerState->AnalyzeSentence(buffer);
		infor->Texture->AnalyzeSentence(buffer);
	}
	fin.close();
	return true;
}
bool ShaderAnalyzer::OutputShaderData(const std::string& name)
{
	string filename = name + ".material";

	ofstream fout;
	fout.open(filename);
	if (!fout.is_open())
	{
		JGLOG(log_Warning, "ShaderAnalyzer::OutputShaderData", filename);
		return false;
	}
	
	for (auto& iter : m_mInformation)
	{
		fout << "ShaderType : " << (int)iter.first << endl;
		fout << "Hlsl Path : " << iter.second.hlslPath << endl;
		iter.second.InputLayout->WriteShaderData(fout);
		iter.second.CBuffer->WriteShaderData(fout);
		iter.second.SamplerState->WriteShaderData(fout);
		iter.second.Texture->WriteShaderData(fout);
		fout << endl;
	}
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