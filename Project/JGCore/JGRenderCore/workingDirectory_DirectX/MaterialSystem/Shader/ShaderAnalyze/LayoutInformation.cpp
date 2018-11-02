#include"LayoutInformation.h"
#include"MaterialSystem/Shader/ShaderTool/InputLayout.h"
using namespace std;
using namespace JGRC;

LayoutInformation::LayoutInformation()
{


}
LayoutInformation::~LayoutInformation()
{


}
void LayoutInformation::AnalyzeSentence(string& sentence)
{
	if (!Decryptable(sentence) || StringUtil::FindString(sentence, "#define"))
	{
		return;
	}
	Start();
	// 부가적인 문장은 건너뛰기
	if (StringUtil::FindString(sentence, "struct") || sentence == "{" || sentence == "};")
	{
		if (sentence == "};")
			Stop(); 
		return;
	}
	// 입력 슬롯 추가
	else if (StringUtil::FindString(sentence, hlslType::INPUTSLOT.c_str()))
	{
		m_bIsInstance = false;
		m_inputSlot = ExtractionBracketNumber(sentence);
		if (m_inputSlot != m_pvInputSlot)
		{
			m_accOffset = 0;
		}
	}
	// 인스턴스 전용 입력 
	else if (StringUtil::FindString(sentence, hlslType::INSTNACE.c_str()))
	{
		m_bIsInstance = true;
		m_InstanceDataSR = ExtractionBracketNumber(sentence);
	}
	else if (StringUtil::FindString(sentence,":"))
	{
		LayoutDesc ly;
		// 입력 슬롯 / 오프셋 / 입력 클래스 설정
		ly.InputSlot = (UINT)m_inputSlot;
		ly.AlignedByteOffset = (UINT)m_accOffset;
		// 인스턴스와 버텍스 입력 레이아웃에 각각 서로 다르게 입력
		if (m_bIsInstance)
		{
			ly.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			ly.InstanceDataStepRate = (UINT)m_InstanceDataSR;
			m_bIsInstance = false;
		}
		else
		{
			ly.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			ly.InstanceDataStepRate = 0;
		}
		// 데이터 형식 받기
		if (StringUtil::FindString(sentence, hlslType::FLOAT.c_str()))
		{
			ly.Format = DXGI_FORMAT_R32_FLOAT;
			m_accOffset += sizeof(float);
		}
		else if (StringUtil::FindString(sentence, hlslType::FLOAT2.c_str()))
		{
			ly.Format = DXGI_FORMAT_R32G32_FLOAT;
			m_accOffset += sizeof(float) * 2;
		}
		else if (StringUtil::FindString(sentence, hlslType::FLOAT3.c_str()))
		{
			ly.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			m_accOffset += sizeof(float) * 3;
		}
		else if (StringUtil::FindString(sentence, hlslType::FLOAT4.c_str()))
		{
			ly.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			m_accOffset += sizeof(float) * 4;
		}
		// 시멘틱 이름 얻기
		uint len = sentence.length();
		uint pos = sentence.find(':');
		string smName = sentence.substr(pos + 1, len - 1);
		StringUtil::DelChar(smName,';');
		StringUtil::DelBlank(smName);
		
		// 뒤에 숫자 확인
		len = smName.length();
		if (isdigit(smName[len - 1]))
		{
			ly.SemanticIndex = atoi(&smName[len - 1]);
			StringUtil::DelChar(smName, smName[len - 1]);
		}
		else
		{
			ly.SemanticIndex = 0;
		}
		
	
		ly.SemanticName = move(smName);
		// 입력 레이아웃 만들기 최종..
		m_vLayout.push_back(ly);
	}
	
}
bool LayoutInformation::Decryptable(const std::string& sentence)
{
	if (StringUtil::FindString(sentence, hlslType::INPUTLAYOUT.c_str()) || IsProgressing())
	{
		return true;
	}
	return false;
}
void LayoutInformation::WriteShaderData(std::ofstream& fout)
{
	fout << "@@ InputLayout" << endl;
	fout << "Count : " << m_vLayout.size() << endl;
	for (auto& iter : m_vLayout)
	{
		fout << iter.SemanticName << " " << iter.SemanticIndex << " " << iter.Format << " "
			 << iter.InputSlot << " " << iter.AlignedByteOffset << " " << iter.InputSlotClass << " "
			 << iter.InstanceDataStepRate << endl;
	}
	fout << "@@" << endl;
}