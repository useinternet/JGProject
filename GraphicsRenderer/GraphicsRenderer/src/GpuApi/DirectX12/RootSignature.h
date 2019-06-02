#pragma once

#include"Dx12Include.h"



namespace GR
{
	namespace Dx12
	{
		class RootSignature
		{
		public:
			enum Range_Type
			{
				SRV,
				UAV,
				CBV,
				SAMPLER
			};
			enum StaticSamplerType
			{
				POINT_WRAP,
				POINT_CLAMP,
				LINEAR_WRAP,
				LINEAR_CLAMP,
				ANISOTROPIC_WRAP,
				ANISOTROPIC_CLAMP
			};
		private:
			ComPtr<ID3D12RootSignature> m_D3DRootSignature;
			std::vector<CD3DX12_ROOT_PARAMETER> m_RootParameters;
			std::vector<D3D12_STATIC_SAMPLER_DESC> m_StaticSamplers;
			std::vector<std::shared_ptr<CD3DX12_DESCRIPTOR_RANGE>> m_RangeMems;
		public:
			void InitConstant(
				uint32_t num32bitValue, uint32_t registerIndex,  uint32_t registerspace = 0,
				D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
			void InitConstantBufferView(uint32_t registerIndex, uint32_t registerspace = 0,
				D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
			void InitShaderResourceView(uint32_t registerIndex, uint32_t registerspace = 0,
				D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
			void InitUnorderedAccessView(uint32_t registerIndex, uint32_t registerspace = 0,
				D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
			void InitDescriptorTable(Range_Type type, uint32_t numDescriptor, uint32_t registerIndex, uint32_t registerspace = 0,
				D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
			void AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC& desc);
			void AddStaticSampler(uint32_t registerIndex, StaticSamplerType type);
			void Finalize();

			ID3D12RootSignature* GetD3DRootSignature() {
				return m_D3DRootSignature.Get();
			}
			const ID3D12RootSignature* GetD3DRootSignature() const {
				return m_D3DRootSignature.Get();
			}
		};

	}
}