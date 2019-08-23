#pragma once

#include "DxObject.h"

namespace RE
{
	class RootSignature;
	class RootParameter
	{
		friend RootSignature;
	public:
		void InitAsConstantBufferView(
			uint32_t register_shader,
			uint32_t register_space = 0,
			D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			m_Param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			m_Param.ShaderVisibility = visibility;
			m_Param.Descriptor.ShaderRegister = register_shader;
			m_Param.Descriptor.RegisterSpace = register_space;
		}


		void InitAsConstants(
			uint32_t num32BitValues,
			uint32_t register_shader,
			uint32_t register_space = 0,
			D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			m_Param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			m_Param.Constants.Num32BitValues = num32BitValues;
			m_Param.Constants.ShaderRegister = register_shader;
			m_Param.Constants.RegisterSpace = register_space;
			m_Param.ShaderVisibility = visibility;
		}

		void InitAsSRV(
			uint32_t register_shader,
			uint32_t register_space = 0,
			D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			m_Param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			m_Param.Descriptor.ShaderRegister = register_shader;
			m_Param.Descriptor.RegisterSpace = register_space;
			m_Param.ShaderVisibility = visibility;
		}


		void InitAsUAV(
			uint32_t register_shader,
			uint32_t register_space = 0,
			D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			m_Param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
			m_Param.Descriptor.ShaderRegister = register_shader;
			m_Param.Descriptor.RegisterSpace = register_space;
			m_Param.ShaderVisibility = visibility;
		}

		void PushAsDescriptorRange(
			D3D12_DESCRIPTOR_RANGE_TYPE type,
			uint32_t numDescriptor,
			uint32_t register_shader,
			uint32_t register_space = 0)
		{
			CD3DX12_DESCRIPTOR_RANGE range;
			range.Init(type, numDescriptor, register_shader, register_space);
			m_Ranges.push_back(range);
		}

		void InitAsDescriptorTable(D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			if (m_Ranges.empty())
				return;
			m_Param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			m_Param.DescriptorTable.NumDescriptorRanges = (uint32_t)m_Ranges.size();
			m_Param.DescriptorTable.pDescriptorRanges = m_Ranges.data();
		}

	private:
		D3D12_ROOT_PARAMETER m_Param;
		std::vector<D3D12_DESCRIPTOR_RANGE> m_Ranges;
	};
	class RootSignature : public DxObject
	{
	public:
		RootSignature(uint32_t numParam);
		RootSignature(const std::string& name);

		void AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC& desc)
		{
			m_Static_Samplers.push_back(desc);
		}
		RootParameter& InitParam(uint32_t idx) {
			return m_Parameters[idx];
		}
	
		ID3D12RootSignature* GetD3DRootSignature() const {
			return m_D3D_RootSignature.Get();
		}
		bool IsVaild() const {
			return m_D3D_RootSignature != nullptr;
		}


		bool Finalize();
	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_D3D_RootSignature;
		std::vector<D3D12_STATIC_SAMPLER_DESC> m_Static_Samplers;
		std::vector<RootParameter> m_Parameters;
	};
}