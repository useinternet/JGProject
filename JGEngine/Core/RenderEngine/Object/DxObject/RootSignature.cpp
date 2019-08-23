#include "pch.h"
#include "RootSignature.h"

using namespace std;

namespace RE
{
	RootSignature::RootSignature(uint32_t param) : DxObject("RootSignature")
	{
		m_Parameters.resize(param);
	}
	RootSignature::RootSignature(const std::string& name) : 
		DxObject(name)
	{
		
	}

	bool RootSignature::Finalize()
	{
		uint32_t param_size = (uint32_t)m_Parameters.size();
		std::vector<D3D12_ROOT_PARAMETER> dx_param(param_size);
		for (uint32_t i = 0; i < param_size; ++i)
		{
			dx_param[i] = m_Parameters[i].m_Param;
		}




		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
			param_size,
			dx_param.data(),
			(uint32_t)m_Static_Samplers.size(),
			m_Static_Samplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	
		ComPtr<ID3DBlob> serializedRootSig = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;


		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (FAILED(hr))
			return false;


		hr = GetD3DDevice()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(m_D3D_RootSignature.GetAddressOf()));

		if (FAILED(hr))
			return false;

		return true;
	}

}