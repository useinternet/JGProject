#pragma once
#include"ShaderRootSignature.h"

namespace JGRC
{
	enum class RCORE_EXPORT EBlurShaderSlot
	{
		cbSettings = 0,
		srvInput,
		uavOutput,
		Count
	};
	class RCORE_EXPORT BlurShaderRootSignature : public ShaderRootSignatureBase
	{
	protected:
		virtual void RootSignature() override 
		{ 
			CD3DX12_DESCRIPTOR_RANGE srvTable;
			srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			CD3DX12_DESCRIPTOR_RANGE uavTable;
			uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

			CD3DX12_ROOT_PARAMETER slotRootParameter[(UINT)EBlurShaderSlot::Count];
			slotRootParameter[(UINT)EBlurShaderSlot::cbSettings].InitAsConstants(12, 0);
			slotRootParameter[(UINT)EBlurShaderSlot::srvInput].InitAsDescriptorTable(1, &srvTable);
			slotRootParameter[(UINT)EBlurShaderSlot::uavOutput].InitAsDescriptorTable(1, &uavTable);

			CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter,
				0, nullptr,
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
			CreateRootSignature(&rootSigDesc);
		}
		virtual void SetInputLayout(InputLayoutDescList& DescList) override
		{
		
		
		}
	};
}