#include"CBufferInformation.h"

using namespace JGRC;
using namespace std;

uint CBuffer::size() const
{
	uint result = 0;
	for (auto& iter : mVars)
	{
		result += iter.second.VarSize;
	}
	return result;
}
CBufferInformation::CBufferInformation()
{

}
CBufferInformation::~CBufferInformation()
{

}

void CBufferInformation::AnalyzeSentence(std::string& sentence)
{
	Start();
	if (StringUtil::FindString(sentence, "struct") || sentence == "{" || sentence == "};" || sentence == "}")
	{
		if (sentence == "};" || sentence == "}")
		{
			m_mCBuffers.insert(pair<uint, CBuffer>(m_bufferNum, m_buffer));
			Stop();
			CBufferClear();
		}
		return;
	}
	if (StringUtil::FindString(sentence, "cbuffer"))
	{
		StringUtil::DelString(sentence, "cbuffer");
		StringUtil::DelBlank(sentence);
		m_buffer.CBufferName = sentence;
	}
	// 공백은 그냥 내보낸다.
	else if(!BlankCheck(sentence) && sentence != "")
	{
		CBufferVar var;
		var.VarName = sentence;
		// 형식을 알아낸다.( 변수 사이즈와 값 )
		if (StringUtil::FindString(sentence, hlslType::MATRIX.c_str()))
		{
			var.VarSize = sizeof(float) * 16;
			StringUtil::DelString(var.VarName, hlslType::MATRIX.c_str());
		}
		else if (StringUtil::FindString(sentence, hlslType::FLOAT4.c_str()))
		{
			var.VarSize = sizeof(float) * 4;
			StringUtil::DelString(var.VarName, hlslType::FLOAT4.c_str());
		}
		else if (StringUtil::FindString(sentence, hlslType::FLOAT3.c_str()))
		{
			var.VarSize = sizeof(float) * 3;
			StringUtil::DelString(var.VarName, hlslType::FLOAT3.c_str());
		}
		else if (StringUtil::FindString(sentence, hlslType::FLOAT2.c_str()))
		{
			var.VarSize = sizeof(float) * 2;
			StringUtil::DelString(var.VarName, hlslType::FLOAT2.c_str());
		}
		else if (StringUtil::FindString(sentence, hlslType::FLOAT.c_str()))
		{
			var.VarSize = sizeof(float);
			StringUtil::DelString(var.VarName, hlslType::FLOAT.c_str());
		}
		for (uint i = 0; i < var.VarSize; ++i)
		{
			var.vValue.push_back(0.0f);
		}
		// 변수 이름 추출
		StringUtil::DelString(var.VarName, ";");
		StringUtil::DelBlank(var.VarName);

		// 변수 상수버퍼에 삽입.
		m_buffer.mVars.insert(pair<uint,CBufferVar>(m_varNum++, var));
	}
}
const CBuffer& CBufferInformation::GetCBuffer(const uint idx)
{
	return m_mCBuffers[idx];
}
bool CBufferInformation::BlankCheck(const std::string& sentence)
{
	for (uint i = 0; i < sentence.length(); ++i)
	{
		if (sentence[i] != ' ')
		{
			return false;
		}
	}
	return true;
}
void CBufferInformation::CBufferClear()
{
	m_buffer.CBufferName = "";
	m_buffer.mVars.clear();
	m_bufferNum++;
	m_varNum = 0;
}

