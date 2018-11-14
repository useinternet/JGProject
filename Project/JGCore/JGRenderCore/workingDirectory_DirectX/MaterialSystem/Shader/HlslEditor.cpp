#include"HlslEditor.h"
#include"DirectX/DirectX.h"
using namespace JGRC;
using namespace std;

HlslEditor::HlslEditor()
{

}
HlslEditor::HlslEditor(const HlslEditor& copy)
{
	m_InputLayout = copy.m_InputLayout;
	m_CBuffers = copy.m_CBuffers;
	m_Textures = copy.m_Textures;
	m_SamplerState = copy.m_SamplerState;
}
HlslEditor::HlslEditor(HlslEditor&& he)
{
	m_InputLayout = he.m_InputLayout;
	m_CBuffers = he.m_CBuffers;
	m_Textures = he.m_Textures;
	m_SamplerState = he.m_SamplerState;

	he.m_InputLayout = nullptr;
	he.m_Textures = nullptr;
	he.m_SamplerState = nullptr;
	m_CBuffers.clear();
}
HlslEditor::~HlslEditor()
{
	GetDx()->DeleteObject(m_InputLayout);
	GetDx()->DeleteObject(m_Textures);
	GetDx()->DeleteObject(m_SamplerState);
	for (auto& iter : m_CBuffers)
	{
		GetDx()->DeleteObject(iter);
	}
}
HlslEditor& HlslEditor::operator=(const HlslEditor& copy)
{
	if (this == &copy)
	{
		return *this;
	}
	m_InputLayout = copy.m_InputLayout;
	m_CBuffers = copy.m_CBuffers;
	m_Textures = copy.m_Textures;
	m_SamplerState = copy.m_SamplerState;
	return *this;
}
HlslEditor& HlslEditor::operator=(HlslEditor&& he)
{
	if (this == &he)
	{
		return *this;
	}
	m_InputLayout = he.m_InputLayout;
	m_CBuffers = he.m_CBuffers;
	m_Textures = he.m_Textures;
	m_SamplerState = he.m_SamplerState;

	he.m_InputLayout = nullptr;
	he.m_Textures = nullptr;
	he.m_SamplerState = nullptr;
	m_CBuffers.clear();
	return  *this;
}
void HlslEditor::RegisterHlslPath(const EShaderType type, const std::string& path)
{
	m_Path = path;
	m_ShaderType = type;
}
InputLayout*  HlslEditor::CreateInputLayout()
{
	if (m_ShaderType != EShaderType::Vertex)
	{
		JGLOG(log_Warning, "JGRC::HlslEditor","hlslEditor Type is not VertexShaderType when Create InputLayout")
	}
	m_InputLayout = GetDx()->CreateObject<InputLayout>();
	return m_InputLayout;
}
CBuffer*  HlslEditor::CreateCBuffer()
{
	CBuffer* buffer = GetDx()->CreateObject<CBuffer>();
	m_CBuffers.push_back(buffer);
	return buffer;
}
Texture2D*    HlslEditor::CreateTexture2D()
{
	if (m_ShaderType != EShaderType::Pixel)
	{
		JGLOG(log_Warning, "JGRC::HlslEditor", "hlslEditor Type is not PixelShaderType when Create Texture2D")
	}
	m_Textures = GetDx()->CreateObject<Texture2D>();
	return m_Textures;
}
SamplerState* HlslEditor::CreateSamplerState()
{
	if (m_ShaderType != EShaderType::Pixel)
	{
		JGLOG(log_Warning, "JGRC::HlslEditor", "hlslEditor Type is not PixelShaderType when Create SamplerState")
	}
	m_SamplerState = GetDx()->CreateObject<SamplerState>();
	return m_SamplerState;
}
void HlslEditor::WriteData(std::ofstream& fout)
{
	fout << "##ShaderType : " << (int)m_ShaderType << endl;
	fout << "##HlslPath : " << m_Path << endl;
	fout << "##InputLayout" << endl;
	fout << "##Count : ";
	if (m_InputLayout)
	{
		vector<D3D11_INPUT_ELEMENT_DESC> descs = m_InputLayout->GetArray();
		fout << descs.size() << endl;

		for (auto& desc : descs)
		{
			fout << desc.SemanticName << " " << desc.SemanticIndex     << " " << desc.Format         << " "
				 << desc.InputSlot    << " " << desc.AlignedByteOffset << " " << desc.InputSlotClass << " "
				 << desc.InstanceDataStepRate << endl;
		}
	}
	else
	{
		fout << 0 << endl;
	}
	fout << "##CBuffers" << endl;
	fout << "##Count : ";
	if (!m_CBuffers.empty())
	{
		fout << m_CBuffers.size() << endl;
		for (auto& buffer : m_CBuffers)
		{
			fout << "##BufferName : " << buffer->GetName() << endl;
			fout << "##VarCount : " << buffer->GetVarArray().size() << endl;

			std::vector<CBuffer::Var> vars = buffer->GetVarArray();
			for (auto& var : vars)
			{
				fout << "##Var : " << var.Name << " " << var.BtStart << " " << var.BtEnd << endl;
			}
		}
	}
	else
	{
		fout << 0 << endl;
	}
	fout << "##Texture2D" << endl;
	fout << "##Count : ";
	if (m_Textures)
	{
		fout << m_Textures->Size() << endl;
		for (auto& tex : m_Textures->GetArray())
		{
			fout << tex << endl;
		}
	}
	else
	{
		fout << 0 << endl;
	}
	fout << "##SamplerState" << endl;
	fout << "##Count : ";
	if (m_SamplerState)
	{
		fout << m_SamplerState->GetArray().size() << endl;
		for (auto& sampler : m_SamplerState->GetArray())
		{
			fout << sampler.Filter   << " " << sampler.AddressU   << " " << sampler.AddressV      << " "
				 << sampler.AddressW << " " << sampler.MipLODBias << " " << sampler.MaxAnisotropy << " "
				 << sampler.BorderColor[0] << " " << sampler.BorderColor[1] << " " << sampler.BorderColor[2] << " "
				 << sampler.BorderColor[3] << " " << sampler.MinLOD         << " " << sampler.MaxLOD         << " " 
				 << endl;
		}
	}
	else
	{
		fout << 0 << endl;
	}
}
////////////////////////////////////////////
////////////// InputLayout /////////////////
////////////////////////////////////////////
void InputLayout::AddDesc(const char* SemanticName, const UINT SemanticIndex, DXGI_FORMAT Format,
	UINT InputSlot, UINT Offset, D3D11_INPUT_CLASSIFICATION InputClass, UINT IDStepData)
{
	m_SementicName.push_back(SemanticName);
	D3D11_INPUT_ELEMENT_DESC desc;
	desc.SemanticName = m_SementicName.back().c_str();
	desc.SemanticIndex = SemanticIndex;
	desc.Format = Format;
	desc.InputSlot = InputSlot;
	desc.AlignedByteOffset = Offset;
	desc.InputSlotClass = InputClass;
	desc.InstanceDataStepRate = IDStepData;
	m_Descs.push_back(desc);
	// 재할당이 일어날 경우에는 다시 지정
	if (m_SementicName.capacity() < m_SementicName.size())
	{
		for (int i = 0; i < m_SementicName.size(); ++i)
		{
			m_Descs[i].SemanticName = m_SementicName[i].c_str();
		}
	}
}
////////////////////////////////////////////
//////////////   CBuffer   /////////////////
////////////////////////////////////////////
bool  CBuffer::SetParam(const std::string& varName, void* data, const uint count)
{
	for (auto& var : m_Vars)
	{
		if (var.Name == varName)
		{
			real* rdata = (real*)data;
			for (uint i = 0; i < count; ++i)
			{
				m_Data[i + var.BtStart] = rdata[i];
			}
			return true;
		}
	}
	return false;
}
uint  CBuffer::GetParam(const std::string& varName, real* data)
{
	uint Count = 0;
	for (auto& var : m_Vars)
	{
		if (var.Name == varName)
		{
			Count = var.BtEnd - var.BtStart + 1;
			data = &m_Data[var.BtStart];
		}
	}
	return Count;
}
void CBuffer::AddVar(const string& Name, const unsigned int BtStart, const unsigned int BtEnd)
{
	Var v;
	v.Name = Name;
	v.BtStart = BtStart; v.BtEnd = BtEnd;
	m_Vars.push_back(v);
}
////////////////////////////////////////////
//////////////  Texture2D  /////////////////
////////////////////////////////////////////
void Texture2D::Add(const string& Name)
{
	m_TextureNames.push_back(Name);
}
ID3D11ShaderResourceView* Texture2D::GetResource(const std::string& name)
{
	uint count = 0;
	for (auto& iter : m_TextureNames)
	{
		if (iter == name)
		{
			return m_SRVs[count];
		}
		count++;
	}
	return nullptr;
}
ID3D11ShaderResourceView** Texture2D::GetResourceAddress(const std::string& name)
{
	uint count = 0;
	for (auto& iter : m_TextureNames)
	{
		if (iter == name)
		{
			return &m_SRVs[count];
		}
		count++;
	}
	return nullptr;
}
void Texture2D::SetResource(ID3D11ShaderResourceView* srv, const std::string& name)
{
	uint count = 0;
	for (auto& iter : m_TextureNames)
	{
		if (iter == name)
		{
			m_SRVs[count] = srv;
			return;
		}
		count++;
	}
	JGLOG(log_Error, "JGRC::Texture2D", name + " is not exist");
}
////////////////////////////////////////////
////////////// SamplerState ////////////////
////////////////////////////////////////////
bool SamplerState::CreateSamplerState()
{
	HRESULT result = S_OK;
	for (uint i = 0; i < Descs.size(); ++i)
	{
		Microsoft::WRL::ComPtr<ID3D11SamplerState> Sampler;
		result = GetDx()->GetDevice()->CreateSamplerState(&Descs[i], Sampler.GetAddressOf());
		if (FAILED(result))
		{
			JGLOG(log_Error, "JGRC::SamplerState", "Failed Create SamplerState");
			return false;
		}
		m_SamplerStates.push_back(move(Sampler));
	}
	return true;
}
void SamplerState::AddDefaultWrapMode()
{
	D3D11_SAMPLER_DESC desc;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MipLODBias = 0;
	desc.MaxAnisotropy = 1;
	desc.BorderColor[0] = 0; desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0; desc.BorderColor[3] = 0;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	Descs.push_back(desc);
}
void SamplerState::AddDefaultClampMode()
{
	D3D11_SAMPLER_DESC desc;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MipLODBias = 0;
	desc.MaxAnisotropy = 1;
	desc.BorderColor[0] = 0; desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0; desc.BorderColor[3] = 0;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	Descs.push_back(desc);
}
void SamplerState::AddCustomMode(const D3D11_SAMPLER_DESC& desc)
{
	Descs.push_back(desc);
}
ID3D11SamplerState** SamplerState::GetAddress(const uint idx)
{
	return m_SamplerStates[idx].GetAddressOf();
}
ID3D11SamplerState*  SamplerState::Get(const uint idx)
{
	return m_SamplerStates[idx].Get();
}