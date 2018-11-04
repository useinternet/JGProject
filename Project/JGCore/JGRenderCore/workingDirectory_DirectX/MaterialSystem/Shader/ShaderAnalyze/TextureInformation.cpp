#include"TextureInformation.h"
#include<cstring>
using namespace JGRC;
using namespace std;

TextureInformation::TextureInformation()
{

}
TextureInformation::~TextureInformation()
{

}
void TextureInformation::AnalyzeSentence(std::string& sentence)
{
	if (!Decryptable(sentence) || StringUtil::FindString(sentence, "#define"))
	{
		return;
	}
	Start();
	if (StringUtil::FindString(sentence, hlslType::TEXTURE_End.c_str()))
	{
		Stop();
		return;
	}
	else if (StringUtil::FindString(sentence, hlslType::TEXTURE_Config.c_str()))
	{
		ExtractionTextureName(sentence);
	}
}
bool TextureInformation::Decryptable(const std::string& sentence)
{
	if (StringUtil::FindString(sentence, hlslType::TEXTURE_Start.c_str()) || IsProgressing())
	{
		return true;
	}
	return false;
}
void TextureInformation::WriteShaderData(ofstream& fout)
{
	uint count = 0;
	fout << "@@Texture" << endl;
	fout << "%TextureCount : " << m_mInformation.size() << endl;
	for (auto& iter : m_mInformation)
	{
		fout << count++ << " " << iter.second << endl;
	}
	fout << "@@" << endl;
}
void TextureInformation::ExtractionTextureName(std::string& s)
{
	bool StartPos = false;
	string tmpStr = "";
	for (auto iter = s.begin(); iter < s.end(); ++iter)
	{
		if (*iter == ')')
			break;
		if (StartPos)
		{
			tmpStr += *iter;
		}
		if (*iter == '(')
			StartPos = true;
	}
	StringUtil::DelBlank(tmpStr);
	uint Pos = tmpStr.find(',');
	
	uint number = atoi(tmpStr.substr(0, Pos).c_str());
	string name = tmpStr.substr(Pos + 1, tmpStr.length() - 2);
	m_mInformation.insert(pair<uint, string>(number, name));
}