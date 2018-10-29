#include"SamplerState.h"
#include"DirectX/DirectX.h"
#include"DirectX/JGDevice.h"
using namespace JGRC;
using namespace Microsoft::WRL;
using namespace std;
SamplerState* SamplerState::instance = nullptr;

SamplerState::SamplerState() {}
SamplerState::~SamplerState() {}
SamplerState* SamplerState::GetInstance() 
{
	if (instance == nullptr)
	{
		instance = new SamplerState;
	}
	return instance;
}
void SamplerState::Release()
{
	if (instance)
	{
		delete instance;
		instance = nullptr;
	}
}
bool SamplerState::AddSampler(const wstring& SamplerName, const ESamplerType type,const D3D11_FILTER filter, const float MipLODBias, const UINT MaxAnisotropy,
	D3D11_COMPARISON_FUNC func ,const jgVec4& color, const float MinLOD, const float MaxLOD)
{
	auto result = m_umSamplerStates.find(SamplerName);
	if (result != m_umSamplerStates.end())
	{
		return false;
	}
	D3D11_SAMPLER_DESC samplerDesc;
	ComPtr<ID3D11SamplerState> sampler;

	samplerDesc.Filter = filter;
	samplerDesc.MipLODBias = MipLODBias;
	samplerDesc.MaxAnisotropy = MaxAnisotropy;
	samplerDesc.ComparisonFunc = func;
	samplerDesc.BorderColor[0] = color.x;
	samplerDesc.BorderColor[1] = color.y;
	samplerDesc.BorderColor[2] = color.z;
	samplerDesc.BorderColor[3] = color.w;
	samplerDesc.MinLOD = MinLOD;
	samplerDesc.MaxLOD = MaxLOD;

	switch (type)
	{
	case ESamplerType::Wrap:
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case ESamplerType::Clamp:
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	}
	DirectX* direct = DirectX::GetInstance();
	HRESULT hresult = direct->GetDevice()->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());
	if (FAILED(hresult))
	{
		return false;
	}
	m_umSamplerStates.insert(pair<wstring, ComPtr<ID3D11SamplerState>>(SamplerName, move(sampler)));
	return true;
}
ID3D11SamplerState* SamplerState::Get(const std::wstring& name)
{
	auto result = m_umSamplerStates.find(name);
	if (result != m_umSamplerStates.end())
	{
		return m_umSamplerStates[name].Get();
	}
	return  nullptr;
}
ID3D11SamplerState** SamplerState::GetAddress(const std::wstring& name)
{
	auto result = m_umSamplerStates.find(name);
	if (result != m_umSamplerStates.end())
	{
		return m_umSamplerStates[name].GetAddressOf();
	}
	return  nullptr;
}