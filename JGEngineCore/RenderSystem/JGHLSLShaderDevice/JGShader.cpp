#include"JGShader.h"
#include"../JGDeviceD.h"
#include"JGBufferManager.h"
#include"JGBuffer.h"
#include"../../EngineStatics/JGLog.h"


using namespace Microsoft::WRL;
using namespace std;


JGShader::JGShader()
{

}
JGShader::~JGShader()
{


}
bool JGShader::Create(JGDeviceD* Device, JGBufferManager* BufferManager)
{
	m_pBufferManager = BufferManager;
	m_pDevice = Device;
	bool result = CreateShader();
	if (!result)
	{
		return false;
	}
	return true;
}
void JGShader::Render(JGShaderData* Data, JGTexture* Texture)
{
	JGShaderDataSetting(Data);
	JGTextureSetting(Texture);

	// 정점 입력 레이아웃 입력
	m_pDevice->GetContext()->IASetInputLayout(m_InputLayout.Get());
	// 정점 셰이더 및 픽셀 셰이더 입력
	m_pDevice->GetContext()->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	m_pDevice->GetContext()->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	for (auto& iter : m_vSamplerStates)
	{
		m_pDevice->GetContext()->PSSetSamplers(0, 1, iter.second.GetAddressOf());
	}
	m_VertexNumber = 0;
	m_PixelNumber = 0;
}
bool JGShader::CreateShader()
{
	return true;
}
void JGShader::JGShaderDataSetting(JGShaderData* Data) {}
void JGShader::JGTextureSetting(JGTexture* Texture) {}
void JGShader::AddInputElement(const char* SemanticName, UINT SemanticIndex, DXGI_FORMAT Format,
	UINT InputSlot, D3D11_INPUT_CLASSIFICATION InputSlotClass, UINT InstanceDataStepRate)
{
	D3D11_INPUT_ELEMENT_DESC Desc = {};

	m_ElementDesc.size() == 0 ?
		Desc.AlignedByteOffset = 0 : Desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	Desc.SemanticName  = SemanticName;
	Desc.SemanticIndex = SemanticIndex;
	Desc.Format = Format;
	Desc.InputSlot = InputSlot;
	Desc.InputSlotClass = InputSlotClass;
	Desc.InstanceDataStepRate = InstanceDataStepRate;


	m_ElementDesc.push_back(move(Desc));
}

void JGShader::AddConstantBuffer(const wstring& BufferName, size_t size)
{
	m_pBufferManager->AddBuffer(BufferName, EJGBufferType::ConstantBuffer, 
		EJGUsageType::Dynamic , EJGCPUType::Access_Write, nullptr, size);
}

void JGShader::AddSamplerState(JGDeviceD* Device, const std::wstring& name, ESamplerType type)
{
	ComPtr<ID3D11SamplerState> SamplerState;
	D3D11_SAMPLER_DESC desc;
	switch (type)
	{
	case ESamplerType::Wrap:
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case ESamplerType::Clamp:
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	}
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MipLODBias = 0.0f;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.BorderColor[0] = 0.0f;
	desc.BorderColor[1] = 0.0f;
	desc.BorderColor[2] = 0.0f;
	desc.BorderColor[3] = 0.0f;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	Device->GetDevice()->CreateSamplerState(&desc, SamplerState.GetAddressOf());
	m_vSamplerStates.push_back(D3DSamplerState(name, move(SamplerState)));
}

void JGShader::Write_RegisterBuffer(const wstring& BufferName, JGShaderData* Data, EJGMapType MapType, ERegisterType RegisterType)
{
	m_pBufferManager->Write(BufferName, MapType, Data->GetData(BufferName));

	if (RegisterType == ERegisterType::Vertex)
	{
		m_pDevice->GetContext()->VSSetConstantBuffers(m_VertexNumber, 1,
			m_pBufferManager->GetBuffer(BufferName)->GetAddress());
		m_VertexNumber++;
	}
	if (RegisterType == ERegisterType::Pixel)
	{
		m_pDevice->GetContext()->PSSetConstantBuffers(m_PixelNumber, 1,
			m_pBufferManager->GetBuffer(BufferName)->GetAddress());
		m_PixelNumber++;
	}
}
JGBufferManager* JGShader::GetBufferManager()
{
	return m_pBufferManager;
}
JGDeviceD* JGShader::GetJGDevice()
{
	return m_pDevice;
}
ID3D11VertexShader* JGShader::GetVertexShader()
{
	return m_VertexShader.Get();
}
ID3D11VertexShader** JGShader::GetVertexShaderAddress()
{
	return m_VertexShader.GetAddressOf();
}
ID3D11PixelShader* JGShader::GetPixelShader()
{
	return m_PixelShader.Get();
}
ID3D11PixelShader** JGShader::GetPixelShaderAddress()
{
	return m_PixelShader.GetAddressOf();
}
ID3D11InputLayout* JGShader::GetInputLayout()
{
	return m_InputLayout.Get();
}
ID3D11InputLayout** JGShader::GetInputLayoutAddress()
{
	return m_InputLayout.GetAddressOf();
}
D3D11_INPUT_ELEMENT_DESC * JGShader::GetInputElements()
{
	return &m_ElementDesc[0];
}
UINT JGShader::GetElementCount()
{
	return (UINT)m_ElementDesc.size();
}
ID3D11SamplerState* JGShader::GetSamplerState(const std::wstring& name)
{
	for (auto& iter : m_vSamplerStates)
	{
		if (name == iter.first)
		{
			return iter.second.Get();
		}
	}
	JGLog::Write(ELogLevel::Warning, TT("%s of SamplerState is not exist"), name.c_str());
	return nullptr;
}
ID3D11SamplerState** JGShader::GetSamplerStateAddress(const std::wstring& name)
{
	for (auto& iter : m_vSamplerStates)
	{
		if (name == iter.first)
		{
			return iter.second.GetAddressOf();
		}
	}
	JGLog::Write(ELogLevel::Warning, TT("%s of SamplerState is not exist"), name.c_str());
	return nullptr;
}
