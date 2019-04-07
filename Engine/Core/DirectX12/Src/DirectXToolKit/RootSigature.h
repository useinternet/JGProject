#pragma once


#include"DirectXCommon/DirectXCommon.h"

namespace Dx12
{
	class RootSignature
	{
	public:
		struct RootSigDescriptorTableData
		{
			uint32_t paramIndex;
			uint32_t numDescriptor;
		};
		using RootSigDescriptorTableDatas = std::vector<RootSigDescriptorTableData>;
	private:
		ComPtr<ID3D12RootSignature> m_RootSignature;
		std::vector<std::unique_ptr<CD3DX12_DESCRIPTOR_RANGE>> m_DescriptorRanges;
		std::vector<CD3DX12_ROOT_PARAMETER>  m_RootParameters;
		std::vector<D3D12_STATIC_SAMPLER_DESC> m_SamplerDescs;
		RootSigDescriptorTableDatas m_RootSigDescriptorTableDatas;

		uint32_t m_RootParamOffset = 0;
	public:
		void InitAsConstantBufferView(UINT registerIndex, UINT registerspace = 0,
			D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsConstant(UINT num32bitValue, UINT registerIndex, UINT registerspace = 0,
			D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsShaderResourceView(UINT registerIndex, UINT registerspace = 0,
			D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsUnorderedAccessView(UINT registerIndex, UINT registerspace = 0,
			D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE type,
			UINT numDescriptor, UINT registerIndex, UINT registerspace = 0,
			D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void AddSampler(const D3D12_STATIC_SAMPLER_DESC& desc);
		void Finalize();
		
		ID3D12RootSignature* Get() const;

		const RootSigDescriptorTableDatas& GetDescriptorTableDatas() const {
			return m_RootSigDescriptorTableDatas;
		}
	};
}