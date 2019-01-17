#pragma once
#include"ShaderRootSignature.h"

namespace JGRC
{
	enum class RCORE_EXPORT ECommonShaderSlot
	{
		cbPerObject,
		cbPerPass,
		sbMaterialData,
		sbLightData,
		sbInstanceData,
		srvCubeMap,
		srvTexture,
		Count
	};

	class RCORE_EXPORT CommonShaderRootSignature : public ShaderRootSignatureBase
	{
	protected:
		virtual void RootSignature() override
		{
			// Texture 슬롯 생성
			CD3DX12_DESCRIPTOR_RANGE TextureSlot;
			TextureSlot.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				1, 0, 0);
			CD3DX12_DESCRIPTOR_RANGE CubeMapSlot;
			CubeMapSlot.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				1, 2, 1);
			// 셰이더 파라미터 설정
			CD3DX12_ROOT_PARAMETER slotParam[(int)ECommonShaderSlot::Count];
			slotParam[(int)ECommonShaderSlot::cbPerObject].InitAsConstantBufferView(0);
			slotParam[(int)ECommonShaderSlot::cbPerPass].InitAsConstantBufferView(1);
			slotParam[(int)ECommonShaderSlot::sbMaterialData].InitAsShaderResourceView(0, 1);
			slotParam[(int)ECommonShaderSlot::sbLightData].InitAsShaderResourceView(1, 1);
			slotParam[(int)ECommonShaderSlot::sbInstanceData].InitAsShaderResourceView(0, 2);
			slotParam[(int)ECommonShaderSlot::srvCubeMap].InitAsDescriptorTable(1, &CubeMapSlot,  D3D12_SHADER_VISIBILITY_PIXEL);
			slotParam[(int)ECommonShaderSlot::srvTexture].InitAsDescriptorTable(1, &TextureSlot,  D3D12_SHADER_VISIBILITY_PIXEL);
			auto staticSamplers = GetStaticSamplers();
			// 바이트 코드화 
			CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc((UINT)ECommonShaderSlot::Count, slotParam,
				(UINT)staticSamplers.size(), staticSamplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			CreateRootSignature(&rootSigDesc);
		}
		virtual void SetInputLayout(InputLayoutDescList& DescList) override
		{
			DescList =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};
		}
	};
}




