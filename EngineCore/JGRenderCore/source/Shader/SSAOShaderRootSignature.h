#pragma once
#include"ShaderRootSignature.h"


namespace JGRC
{
	enum class ESSAOShaderSlot
	{
		SSAOData,
		Empty_Consant,
		NormalMap,
		DepthMap,
		RandomVecMap
	};
	class RCORE_EXPORT SSAOShaderRootSignature : public ShaderRootSignatureBase
	{
	protected:
		virtual void RootSignature() override
		{
			CD3DX12_DESCRIPTOR_RANGE NormalMapTable;
			NormalMapTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
			CD3DX12_DESCRIPTOR_RANGE DepthMapTable;
			DepthMapTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0);
			CD3DX12_DESCRIPTOR_RANGE RandomVecMapTable;
			RandomVecMapTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0);


			// Root parameter can be a table, root descriptor or root constants.
			CD3DX12_ROOT_PARAMETER slotRootParameter[5];

			// Perfomance TIP: Order from most frequent to least frequent.
			slotRootParameter[0].InitAsConstantBufferView(0);
			slotRootParameter[1].InitAsConstants(1, 1);
			slotRootParameter[2].InitAsDescriptorTable(1, &NormalMapTable, D3D12_SHADER_VISIBILITY_PIXEL);
			slotRootParameter[3].InitAsDescriptorTable(1, &DepthMapTable, D3D12_SHADER_VISIBILITY_PIXEL);
			slotRootParameter[4].InitAsDescriptorTable(1, &RandomVecMapTable, D3D12_SHADER_VISIBILITY_PIXEL);
			auto sampler = staticSamplers();

			// A root signature is an array of root parameters.
			CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(5, slotRootParameter,
				(UINT)sampler.size(), sampler.data(),
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			CreateRootSignature(&rootSigDesc);
		}
		virtual void SetInputLayout(InputLayoutDescList& DescList) override
		{

		}
		std::array<CD3DX12_STATIC_SAMPLER_DESC, 4> staticSamplers()
		{
			const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
				0,
				D3D12_FILTER_MIN_MAG_MIP_POINT,
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
			const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
				1,
				D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
			const CD3DX12_STATIC_SAMPLER_DESC depthMapSam(
				2,
				D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				D3D12_TEXTURE_ADDRESS_MODE_BORDER,
				D3D12_TEXTURE_ADDRESS_MODE_BORDER,
				D3D12_TEXTURE_ADDRESS_MODE_BORDER,
				0.0f,
				0,
				D3D12_COMPARISON_FUNC_LESS_EQUAL,
				D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE);

			const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
				3, // shaderRegister
				D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

			return { pointClamp, linearClamp, depthMapSam, linearWrap };
		}
	};
}