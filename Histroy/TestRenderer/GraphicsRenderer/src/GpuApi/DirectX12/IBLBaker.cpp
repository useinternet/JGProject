#include"pch.h"
#include"IBLBaker.h"
#include"GraphicsDevice.h"
#include"Commander.h"
#include"RootSignatureCache.h"
namespace GR
{
	namespace Dx12
	{
		std::wstring IBLBaker::m_SpBRDFShaderPath = L"SpecularBrdfCS.hlsl";
		std::wstring IBLBaker::m_SpMapShaderPath  = L"SpecularmapCS.hlsl"; 
		std::wstring IBLBaker::m_IrrMapShaderPath = L"IrrmapCS.hlsl"; 

		void IBLBaker::Init()
		{
			auto device = GraphicsDevice::GetPointer();
			auto shaderDir = device->GetComputeShaderDirPath();
			// brdf
			{
				ComputeShader CS;
				RootSignature rootSig = device->GetRootSignatureFromCache(ERootSignature::C_SpecularBRDF);
				device->CreateShader(shaderDir + m_SpBRDFShaderPath, &CS);
				m_SpBRDFPSO.BindComputeShader(CS);
				m_SpBRDFPSO.BindRootSignature(rootSig.GetD3DRootSignature());
				m_SpBRDFPSO.Finalize();
			}
			// sp map
			{
				ComputeShader CS;
				RootSignature rootSig = device->GetRootSignatureFromCache(ERootSignature::C_SpecularMap);
				device->CreateShader(shaderDir + m_SpMapShaderPath, &CS);
				m_SpMapPSO.BindComputeShader(CS);
				m_SpMapPSO.BindRootSignature(rootSig.GetD3DRootSignature());
				m_SpMapPSO.Finalize();
			}
			// irrmap
			{
				ComputeShader CS;
				RootSignature rootSig = device->GetRootSignatureFromCache(ERootSignature::C_IrrMap);
				device->CreateShader(shaderDir + m_IrrMapShaderPath, &CS);
				m_IrrMapPSO.BindComputeShader(CS);
				m_IrrMapPSO.BindRootSignature(rootSig.GetD3DRootSignature());
				m_IrrMapPSO.Finalize();
			}
		}
		bool IBLBaker::Bake(ComputeCommander* commander, const Texture& Intexture,
			Texture* OutbrdfTexture, Texture* OutspMap, Texture* Outirrmap)
		{
			Texture texture = Intexture;
			if (texture->GetDesc().DepthOrArraySize != 6)
				return false;


			auto device = GraphicsDevice::GetPointer();
			auto resourceDesc = texture->GetDesc();
			auto desc = resourceDesc;
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = resourceDesc.Format;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MipLevels = resourceDesc.MipLevels;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			texture.SetSRVDesc(&srvDesc);

			// 府家胶 积己
			{
				desc.Width = 1024; desc.Height = 1024;
				desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
				GPUResource spmapResource = device->CreateGPUResource(desc, nullptr,
					GraphicsDevice::DEFAULT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				*OutspMap = device->CreateTexture(spmapResource);

				//

				desc.Width = 32; desc.Height = 32;
				desc.DepthOrArraySize = 6;
				desc.MipLevels = 1;

				GPUResource irrResource = device->CreateGPUResource(desc, nullptr,
					GraphicsDevice::DEFAULT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				*Outirrmap = device->CreateTexture(irrResource);
			}
			if (!m_SpecularBRDF.IsValid())
			{
				desc.Width = 256; desc.Height = 256;
				desc.DepthOrArraySize = 1;
				desc.Format = DXGI_FORMAT_R16G16_FLOAT;
				GPUResource spbrdfResource = device->CreateGPUResource(desc, nullptr,
					GraphicsDevice::DEFAULT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

				m_SpecularBRDF = device->CreateTexture(spbrdfResource);
				RootSignature rootSig = device->GetRootSignatureFromCache(ERootSignature::C_SpecularBRDF);
				ComputePSO pso = m_SpBRDFPSO;


				commander->SetRootSignature(rootSig);
				commander->SetPipelineState(pso);

				commander->SetUAVDescriptorTable(EC_SpecularBRDF_RootParam_OutputTexture, 1, &m_SpecularBRDF.GetUAV());
				commander->Dispatch(m_SpecularBRDF->GetDesc().Width / 32, m_SpecularBRDF->GetDesc().Height / 32, 1);
				commander->UAVBarrier(m_SpecularBRDF, true);
			}
			*OutbrdfTexture = m_SpecularBRDF;

			// UAV 累己
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = resourceDesc.Format;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.Texture2DArray.ArraySize = 6;
			uavDesc.Texture2DArray.FirstArraySlice = 0;
			uavDesc.Texture2DArray.MipSlice = 0;
			uavDesc.Texture2DArray.PlaneSlice = 0;



			// 风飘 辑疙 棺 PSO



			for (UINT arraySlice = 0; arraySlice < 6; ++arraySlice)
			{
				commander->CopyResourceRegion(*OutspMap, 0, 0, 0, 0, texture, 0, arraySlice);
			}

			commander->TransitionBarrier(*OutspMap, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);



			// prefiltered specular Map
			{
				RootSignature rootSig = device->GetRootSignatureFromCache(ERootSignature::C_SpecularMap);
				ComputePSO pso = m_SpMapPSO;

				commander->SetRootSignature(rootSig);
				commander->SetPipelineState(pso);
				commander->SetSRVDescriptorTable(EC_SpecularMap_RootParam_InputTexture, 1, &texture.GetSRV());

				const float deltaRoughness = 1.0f / std::max(float(resourceDesc.MipLevels - 1), 1.0f);

				for (uint32_t level = 1, size = 512; level < resourceDesc.MipLevels; ++level, size /= 2)
				{
					uavDesc.Texture2DArray.MipSlice = level;
					OutspMap->SetUAVDesc(&uavDesc);
					const UINT numGroups = std::max<UINT>(1, size / 32);
					const float spmapRoughness = level * deltaRoughness;

					float constant_roughness = spmapRoughness;


					commander->SetDynamicConstantBuffer(EC_SpecularMap_RootParam_Roughness, sizeof(float), &constant_roughness);
					commander->SetUAVDescriptorTable(EC_SpecularMap_RootParam_OutputTexture, 1, &OutspMap->GetUAV());
					commander->Dispatch(numGroups, numGroups, 6);

				}
				OutspMap->SetUAVDesc(nullptr);

				commander->TransitionBarrier(*OutspMap, D3D12_RESOURCE_STATE_COMMON, true);
			}

			// Compute diffuse irradiance cubemap
			{
				uavDesc.Texture2DArray.MipSlice = 0;
				Outirrmap->SetUAVDesc(&uavDesc);


				RootSignature rootSig = device->GetRootSignatureFromCache(ERootSignature::C_IrrMap);
				ComputePSO pso = m_IrrMapPSO;
				commander->SetRootSignature(rootSig);
				commander->SetPipelineState(pso);


				commander->SetSRVDescriptorTable(EC_IrrMap_RootParam_InputTexture, 1, &texture.GetSRV());
				commander->SetUAVDescriptorTable(EC_IrrMap_RootParam_OutputTexture, 1, &Outirrmap->GetUAV());
				commander->Dispatch(Outirrmap->GetResource()->GetDesc().Width / 32, Outirrmap->GetResource()->GetDesc().Height / 32, 6);


				commander->UAVBarrier(*Outirrmap, true);

				Outirrmap->SetUAVDesc(nullptr);
			}
			return true;
		}
	}
}