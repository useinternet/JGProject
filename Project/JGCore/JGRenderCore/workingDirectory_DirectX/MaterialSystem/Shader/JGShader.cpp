#include"JGShader.h"
#include"DirectX/DirectX.h"
#include"ShaderReader.h"
using namespace std;
using namespace JGRC;
uint   JGShader::RefCount = 0;
JGShader::unSRVArray JGShader::m_umSRVs;
string JGShader::FunctionName = "main";
string JGShader::VSVersion = "vs_5_0";
string JGShader::PSVersion = "ps_5_0";
UINT   JGShader::Flags = D3D10_SHADER_ENABLE_STRICTNESS;

JGShader::~JGShader()
{
	RefCount--;
	if (ShaderBuffer)
	{
		switch (m_ShaderType)
		{
		case EShaderType::Vertex:
			((ID3D11VertexShader*)ShaderBuffer)->Release();
			break;
		case EShaderType::Pixel:
			((ID3D11PixelShader*)ShaderBuffer)->Release();
			break;
		}
		ShaderBuffer = nullptr;
	}
	GetDx()->DeleteObject(m_InputLayout);
	GetDx()->DeleteObject(m_Texture2D);
	GetDx()->DeleteObject(m_SamplerState);
	for (auto& iter : m_CBufferValue)
	{
		GetDx()->DeleteObject(iter);
	}
	for (auto& buffer : m_ConstantBuffer)
	{
		GetDx()->DeleteObject(buffer);
	}
	// 참조 카운트가 0이 되면
	if (RefCount == 0)
	{
		for (auto& srv : m_umSRVs)
		{
			srv.second->Release();
			srv.second = nullptr;
		}
	}
}
bool  JGShader::AddTexture(const std::string& name, const std::string& path)
{
	if (m_Texture2D == nullptr)
	{
		JGLOG(log_Warning, "JGRC::JGShader", m_Path + " is Shader's Texture2D is nullptr when Add Texture | " + path);
		return false;
	}
	if (m_umSRVs.find(path) != m_umSRVs.end())
	{
		m_Texture2D->AddResource(m_umSRVs[path]);
		return true;
	}

	ID3D11ShaderResourceView* view = nullptr;
	HRESULT result = S_OK;
	wstring wpath;
	wpath.assign(path.begin(), path.end());
	result = D3DX11CreateShaderResourceViewFromFile(GetDx()->GetDevice(),
		wpath.c_str(), nullptr, nullptr, &view, nullptr);
	if (FAILED(result))
	{
		JGLOG(log_Error,"JGRC::JGShader", "Failed Create ResourceView When AddTexture in " + m_Path)
		return false;
	}
	m_umSRVs.insert(unSRVPair(path, view));
	m_Texture2D->AddResource(view);
	return true;
}
bool  JGShader::AddTexture(ID3D11ShaderResourceView* srv)
{
	m_Texture2D->AddResource(srv);
	return true;
}
bool  JGShader::SetTexture(const std::string& name, ID3D11ShaderResourceView* srv)
{
	m_Texture2D->SetResource(srv, name);
	return true;
}
bool  JGShader::SetTexture(const uint idx, ID3D11ShaderResourceView* srv)
{
	m_Texture2D->SetResource(srv, idx);
	return true;
}
uint  JGShader::GetParam(const string& VarName, real* outData)
{
	uint result = 0;
	for (auto& buffer : m_CBufferValue)
	{
		result = buffer->GetParam(VarName, outData);
		if (result != 0)
		{
			break;
		}
	}
	return result;
}
void  JGShader::SetParam(const std::string& VarName, void* data, const uint Count)
{
	for (auto& buffer : m_CBufferValue)
	{
		if (buffer->SetParam(VarName, data, Count))
		{
			break;
		}
	}
}
void  JGShader::Render()
{
	// 상수버퍼 작성
	for (uint i = 0; i < m_ConstantBuffer.size(); ++i)
	{
		m_ConstantBuffer[i]->Write(EMapType::Write_Discard,
			m_CBufferValue[i]->GetData());
		switch (m_ShaderType)
		{
		case EShaderType::Pixel:
			GetDx()->GetContext()->PSSetConstantBuffers((UINT)i, 1, m_ConstantBuffer[i]->GetAddress());
			break;
		case EShaderType::Vertex:
			GetDx()->GetContext()->VSSetConstantBuffers((UINT)i, 1, m_ConstantBuffer[i]->GetAddress());
			break;
		default:
			JGLOG(log_Error, "JGRC::JGShader", "ShaderType is not exist when ShaderRendering");
			break;
		}
	}
	if (m_Texture2D)
	{
		for (uint i = 0; i < m_Texture2D->SRVSize(); ++i)
		{
			GetDx()->GetContext()->PSSetShaderResources((UINT)i, 1, m_Texture2D->GetResourceAddress(i));
		}
	}
	if (ShaderBuffer)
	{
		switch (m_ShaderType)
		{
		case EShaderType::Pixel:
			GetDx()->GetContext()->PSSetShader((ID3D11PixelShader*)(ShaderBuffer), nullptr, 0);
			break;
		case EShaderType::Vertex:
			GetDx()->GetContext()->VSSetShader((ID3D11VertexShader*)(ShaderBuffer), nullptr, 0);
			break;
		}
	}
	else
	{
		JGLOG(log_Error, "JGRC::JGShader", "ShaderBuffer is nullptr | " + m_Path);
	}
	if (m_InputLayout)
	{
		GetDx()->GetContext()->IASetInputLayout(m_InputLayout->GetLayout());
	}
	if (m_SamplerState)
	{
		for (uint i = 0; i < m_SamplerState->Size(); ++i)
		{
			GetDx()->GetContext()->PSSetSamplers((UINT)i, 1, m_SamplerState->GetAddress(i));
		}
	}
}
InputLayout* JGShader::GetLayout()
{
	if (!m_InputLayout)
	{
		m_InputLayout = GetDx()->CreateObject<InputLayout>();
	}
	return m_InputLayout;
}
CBufferArray* JGShader::GetCBufferArray()
{ 
	return &m_CBufferValue; 
}
Texture2D*   JGShader::GetTexture2D()
{ 
	if (!m_Texture2D)
	{
		m_Texture2D = GetDx()->CreateObject<Texture2D>();
	}
	return m_Texture2D; 
}
SamplerState* JGShader::GetSamplerState() 
{ 
	if (!m_SamplerState)
	{
		m_SamplerState = GetDx()->CreateObject<SamplerState>();
	}
	return m_SamplerState;
}

bool JGShader::RegisterBuffer()
{
	bool result = true;
	uint Count = m_CBufferValue.size();

	for (uint i = 0; i < Count; ++i)
	{
		JGBuffer* jgBf = GetDx()->CreateObject<JGBuffer>();
		result = jgBf->CreateBuffer(EBufferType::ConstantBuffer, EUsageType::Dynamic,
			ECPUType::Access_Write, nullptr, m_CBufferValue[i]->GetSize());
		if (!result)
		{
			return false;
		}
		m_ConstantBuffer.push_back(jgBf);
	}
	return true;
}
bool JGShader::Compile(HWND hWnd)
{
	ID3D10Blob* Shader = nullptr;
	ID3D10Blob* Error = nullptr;

	// 버전 체크
	string version;
	wstring wPath;
	wPath.assign(m_Path.begin(), m_Path.end());
	switch (m_ShaderType)
	{
	case EShaderType::Pixel:
		version = PSVersion;
		break;
	case EShaderType::Vertex:
		version = VSVersion;
		break;
	default:
		JGLOG(log_Error, "JGRC::JGShader", "ShaderType is not exist");
		break;
	}
	HRESULT result = S_OK;
	result = D3DX11CompileFromFile(wPath.c_str(), nullptr, nullptr,
		FunctionName.c_str(), version.c_str(), Flags, 0, nullptr, &Shader, &Error, nullptr);
	if (FAILED(result))
	{
		char* compileErrors;
		if (Error)
		{
			compileErrors = (char*)(Error->GetBufferPointer());
			JGLOG(log_Error, "JGRC::JGShader", string(compileErrors));
		}
		else
		{
			JGLOG(log_Error, "JGRC::JGShader", m_Path + " Missing Shader File");
		}
		return false;
	}
	// 버퍼 생성
	switch (m_ShaderType)
	{
	case EShaderType::Vertex:
		result = GetDx()->GetDevice()->CreateVertexShader(Shader->GetBufferPointer(), Shader->GetBufferSize(),
			nullptr, (ID3D11VertexShader**)&(ShaderBuffer));
		break;
	case EShaderType::Pixel:
		result = GetDx()->GetDevice()->CreatePixelShader(Shader->GetBufferPointer(), Shader->GetBufferSize(),
			nullptr, (ID3D11PixelShader**)&(ShaderBuffer));
		break;
	default:
		JGLOG(log_Error, "JGRC::JGShader", "ShaderType is not exist");
		break;
	}
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::JGShader", m_Path + "  Falied Create ShaderBuffer");
		return false;
	}

	if (m_InputLayout)
	{
		GetDx()->GetDevice()->CreateInputLayout(m_InputLayout->Get(), m_InputLayout->Size(),
			Shader->GetBufferPointer(), Shader->GetBufferSize(), m_InputLayout->GetLayoutAddress());
	}
	return true;
}