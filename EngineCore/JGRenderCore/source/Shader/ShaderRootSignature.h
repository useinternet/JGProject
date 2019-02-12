#pragma once
#include<DxCommon/DxCommon.h>

namespace JGRC
{
	typedef std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayoutDescList;
	class RCORE_EXPORT ShaderRootSignatureBase
	{
		ID3D12Device* m_Device;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
		InputLayoutDescList                         m_InputLayout;
		std::string  m_ErrorCode;
	protected:
		void CreateRootSignature(D3D12_ROOT_SIGNATURE_DESC* Desc)
		{
			Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

			HRESULT result = D3D12SerializeRootSignature(Desc, D3D_ROOT_SIGNATURE_VERSION_1,
				serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());
			if (errorBlob != nullptr)
			{
				m_ErrorCode = (char*)errorBlob->GetBufferPointer();
				::OutputDebugStringA(m_ErrorCode.c_str());
			}
			// 루트 서명 생성
			ThrowIfFailed(result);
			ThrowIfFailed(m_Device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
				serializedRootSig->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
		}
		virtual void RootSignature() { }
		virtual void SetInputLayout(InputLayoutDescList& DescList) {}
	public:
		void RootSign(ID3D12Device* Device)
		{
			m_Device = Device;
			RootSignature();
			SetInputLayout(m_InputLayout);
		}
		ID3D12RootSignature* Get() const  { return m_RootSignature.Get(); }
		D3D12_INPUT_ELEMENT_DESC* GetInputLayoutData() { 
			if(m_InputLayout.empty())  
				return nullptr; 
			else 
				return m_InputLayout.data();
		}
		UINT GetInputLayoutSize() const { return (UINT)m_InputLayout.size(); }
	protected:
		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 8> GetStaticSamplers()
		{
			// Applications usually only need a handful of samplers.  So just define them all up front
			// and keep them available as part of the root signature.  

			const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
				0, // shaderRegister
				D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

			const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
				1, // shaderRegister
				D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

			const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
				2, // shaderRegister
				D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

			const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
				3, // shaderRegister
				D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

			const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
				4, // shaderRegister
				D3D12_FILTER_ANISOTROPIC, // filter
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
				0.0f,                             // mipLODBias
				8);                               // maxAnisotropy

			const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
				5, // shaderRegister
				D3D12_FILTER_ANISOTROPIC, // filter
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
				0.0f,                              // mipLODBias
				8);                                // maxAnisotropy
			const CD3DX12_STATIC_SAMPLER_DESC shadow(
				6, // shaderRegister
				D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
				D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
				0.0f,                               // mipLODBias
				16,                                 // maxAnisotropy
				D3D12_COMPARISON_FUNC_LESS_EQUAL,
				D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

			const CD3DX12_STATIC_SAMPLER_DESC cubewrap(
				7, // shaderRegister
				D3D12_FILTER_ANISOTROPIC, // filter
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
				0.0f,                             // mipLODBias
				8);                               // maxAnisotropy
			return {
				pointWrap, pointClamp,
				linearWrap, linearClamp,
				anisotropicWrap, anisotropicClamp,
			    shadow,cubewrap };
		}
	};
}
