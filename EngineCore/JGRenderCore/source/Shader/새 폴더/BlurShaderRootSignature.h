#pragma once
#include"ShaderRootSignature.h"

namespace JGRC
{

	enum class RCORE_EXPORT EBlurShaderSlot
	{
		cbBlur,
		cbBool,
		srvInput,
		Count
	};
	class RCORE_EXPORT BlurShaderRootSignature : public ShaderRootSignatureBase
	{
	protected:
		virtual void RootSignature() override 
		{ 
			CD3DX12_DESCRIPTOR_RANGE srvTable;
			srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

			CD3DX12_ROOT_PARAMETER slotRootParameter[(UINT)EBlurShaderSlot::Count];
			slotRootParameter[(UINT)EBlurShaderSlot::cbBlur].InitAsConstantBufferView(0);
			slotRootParameter[(UINT)EBlurShaderSlot::cbBool].InitAsConstants(1, 1);
			slotRootParameter[(UINT)EBlurShaderSlot::srvInput].InitAsDescriptorTable(1, &srvTable);
		

			CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc((UINT)EBlurShaderSlot::Count, slotRootParameter,
				1,  
				&CD3DX12_STATIC_SAMPLER_DESC(
					0,
					D3D12_FILTER_MIN_MAG_MIP_POINT,
					D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
					D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
					D3D12_TEXTURE_ADDRESS_MODE_CLAMP),
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


			CreateRootSignature(&rootSigDesc);
		}
		virtual void SetInputLayout(InputLayoutDescList& DescList) override
		{
		
		
		}
	};
}