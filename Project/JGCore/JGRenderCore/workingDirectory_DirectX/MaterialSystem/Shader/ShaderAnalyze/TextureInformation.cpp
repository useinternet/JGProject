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
	// 정의 구문이면 바로 함수 에서 탈출
	if (StringUtil::FindString(sentence, "#define"))
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
const string& TextureInformation::GetTextureName(const uint idx) const
{
	return m_mInformation.at(idx);
}
uint TextureInformation::Size() const
{
	return m_mInformation.size();
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
	uint Pos = s.find(',');
	
	uint number = atoi(s.substr(0, Pos - 1).c_str());
	string name = s.substr(Pos + 1, s.length() - 2);
	m_mInformation.insert(pair<uint, string>(number, name));
}