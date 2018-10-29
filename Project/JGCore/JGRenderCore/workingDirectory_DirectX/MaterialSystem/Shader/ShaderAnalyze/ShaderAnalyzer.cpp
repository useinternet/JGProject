#include"ShaderAnalyzer.h"
#include"LayoutInformation.h"
using namespace JGRC;
using namespace std;
ShaderAnalyzer::ShaderAnalyzer()
{

}
ShaderAnalyzer::~ShaderAnalyzer()
{

}

bool ShaderAnalyzer::Analyze(const string& hlslPath, const EShaderType ShaderType)
{
	fstream fin;
	string buffer; // 입력 받아올 문자열
	string path;   // 패쓰
	LayoutInformation infor;
	fin.open(hlslPath.c_str());
	if (!fin.is_open())
	{
		return false;
	}
	while (!fin.eof())
	{

		getline(fin, buffer);
		if (StringUtil::FindString(buffer, "#include"))
		{
			path = IncludeAyz(hlslPath,buffer);
			Analyze(path, ShaderType);
		}
		else if (StringUtil::FindString(buffer, "INPUTLAYOUT") || infor.IsProgressing())
		{
			infor.AnalyzeSentence(buffer);
		}
	}
	fin.close();
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