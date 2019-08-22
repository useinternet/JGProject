#include"pch.h"
#include"RootSignature.h"
#include"Dx12Helper.h"
#include"GraphicsDevice.h"
#include"SamplerState.h"
using namespace std;



namespace GR
{
	namespace Dx12
	{
		void RootSignature::InitConstant(
			uint32_t num32bitValue, uint32_t registerIndex, uint32_t registerspace,
			D3D12_SHADER_VISIBILITY visibility)
		{
			CD3DX12_ROOT_PARAMETER param;
			param.InitAsConstants(num32bitValue, registerIndex, registerspace, visibility);
			m_RootParameters.push_back(param);
		}
		void RootSignature::InitConstantBufferView(uint32_t registerIndex, uint32_t registerspace,
			D3D12_SHADER_VISIBILITY visibility)
		{
			CD3DX12_ROOT_PARAMETER param;
			param.InitAsConstantBufferView(registerIndex, registerspace, visibility);
			m_RootParameters.push_back(param);
		}
		void RootSignature::InitShaderResourceView(uint32_t registerIndex, uint32_t registerspace,
			D3D12_SHADER_VISIBILITY visibility)
		{
			CD3DX12_ROOT_PARAMETER param;
			param.InitAsShaderResourceView(registerIndex, registerspace, visibility);
			m_RootParameters.push_back(param);
		}
		void RootSignature::InitUnorderedAccessView(uint32_t registerIndex, uint32_t registerspace,
			D3D12_SHADER_VISIBILITY visibility)
		{
			CD3DX12_ROOT_PARAMETER param;
			param.InitAsUnorderedAccessView(registerIndex, registerspace, visibility);
			m_RootParameters.push_back(param);
		}
		void RootSignature::InitDescriptorTable(Range_Type type, uint32_t numDescriptor, uint32_t registerIndex, uint32_t registerspace,
			D3D12_SHADER_VISIBILITY visibility)
		{
			D3D12_DESCRIPTOR_RANGE_TYPE d3d_type;
			switch (type)
			{
			case SRV:
				d3d_type = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				break;
			case UAV:
				d3d_type = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				break;
			case CBV:
				d3d_type = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
				break;
			case SAMPLER:
				d3d_type = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
				break;
			}
			CD3DX12_ROOT_PARAMETER param;

			auto range = make_unique< CD3DX12_DESCRIPTOR_RANGE>(d3d_type, numDescriptor, registerIndex, registerspace);
			auto prange = range.get();
			m_RangeMems.push_back(move(range));


			param.InitAsDescriptorTable(1, prange, visibility);
			m_RootParameters.push_back(param);
		}
		void RootSignature::AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC& desc)
		{
			m_StaticSamplers.push_back(desc);
		}
		void RootSignature::AddStaticSampler(uint32_t registerIndex, RootSignature::StaticSamplerType type)
		{
			switch (type)
			{
			case POINT_WRAP:
				m_StaticSamplers.push_back(SamplerState::PointWrap(registerIndex));
				break;
			case POINT_CLAMP:
				m_StaticSamplers.push_back(SamplerState::PointClamp(registerIndex));
				break;
			case LINEAR_WRAP:
				m_StaticSamplers.push_back(SamplerState::LinearWrap(registerIndex));
				break;
			case LINEAR_CLAMP:
				m_StaticSamplers.push_back(SamplerState::LinearClamp(registerIndex));
				break;
			case ANISOTROPIC_WRAP:
				m_StaticSamplers.push_back(SamplerState::AnisotropicWrap(registerIndex));
				break;
			case ANISOTROPIC_CLAMP:
				m_StaticSamplers.push_back(SamplerState::AnisotropicClamp(registerIndex));
				break;
			case SHADOW:
				m_StaticSamplers.push_back(SamplerState::Shadow(registerIndex));
				break;
			}
		}
		void RootSignature::Finalize()
		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
				(uint32_t)m_RootParameters.size(), m_RootParameters.data(),
				(uint32_t)m_StaticSamplers.size(), m_StaticSamplers.data(),
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


			m_D3DRootSignature = CreateD3DRootSignature(GraphicsDevice::GetPointer()->GetD3DDevice(),
				&rootSigDesc);
		}

	}
}