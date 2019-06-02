#include"pch.h"

#include"PSOCache.h"
#include"GraphicsResource.h"
#include"Dx12Helper.h"
#include"Renderer.h"
#include"RootSignature.h"
#include"GraphicsDevice.h"
#include"VertexCollection.h"
#include"BlendState.h"
#include"DepthStencilState.h"
#include"RasterizerState.h"
namespace GR
{
	namespace Dx12
	{
		GraphicsPSO PSOCache::GetGraphicsPSO(RootSignature& rootSig, EGraphicsPSO enumPass, EGraphicsMacroOption option)
		{
			std::lock_guard<std::mutex> lock(m_GraphicsPSOMutex);
			GraphicsPSO pso;
			uint32_t hashCode = (uint32_t)Hash::pair_hash()(std::pair<uint32_t,uint32_t>(enumPass, option));
		
			if (m_GraphicsPSOMap.find(hashCode) != m_GraphicsPSOMap.end())
			{
				return m_GraphicsPSOMap[hashCode];
			}
			pso.BindRootSignature(rootSig);
			switch (enumPass)
			{
			case GBUFFER:
				CreateGBufferPSO(pso, option);
				break;
			case SKYBOX:
				CreateSkyPSO(pso, option);
				break;
			}

			m_GraphicsPSOMap[hashCode] = pso;
			return pso;
		}
		ComputePSO  PSOCache::GetComputePSO(RootSignature& rootSig, EComputePSO enumCompute, EComputeMacroOption option)
		{
			std::lock_guard<std::mutex> lock(m_ComputePSOMutex);

			ComputePSO pso;
			uint32_t hashCode = (uint32_t)Hash::pair_hash()(std::pair<uint32_t, uint32_t>(enumCompute, option));


			if (m_ComputePSOMap.find(hashCode) != m_ComputePSOMap.end())
			{
				return m_ComputePSOMap[hashCode];
			}
			pso.BindRootSignature(rootSig.GetD3DRootSignature());

			switch (enumCompute)
			{
			case EQUIRECT2CUBE:
				CreateEquirect2Cube(pso, option);
				break;
			case GENERATEMIPMAPS:
				CreateGenerateMipMaps(pso, option);
				break;
			case IRRADIANCEMAP:
				CreateIrradianceMap(pso, option);
				break;
			case SPECULARBRDF:
				CreateSpecularBRDF(pso, option);
				break;
			case SPECULARMAP:
				CreateSpecularMap(pso, option);
				break;
			}
			m_ComputePSOMap[hashCode] = pso;


			// ±¸Çö
			return pso;
		}
		void PSOCache::CreateGBufferPSO(GraphicsPSO& pso, EGraphicsMacroOption option)
		{
			VertexShader VS;
			PixelShader PS;
			auto device = GraphicsDevice::GetPointer();
			std::vector<D3D_SHADER_MACRO> macro;

			
			if (option & USE_SKINNED)
			{
				pso.SetInputLayout(SkinnedVertex::GetNumInputLayout(), SkinnedVertex::GetInputLayoutDesc());
				macro.push_back({ "USE_SKINNED", "1" });
			}
			else
			{
				pso.SetInputLayout(Vertex::GetNumInputLayout(), Vertex::GetInputLayoutDesc());
			}
			if (option & USE_ALBEDO)
			{
				macro.push_back({ "USE_ALBEDO", "1" });
			}
			if (option & USE_NORMAL)
			{
				macro.push_back({ "USE_NORMAL", "1" });
			}
			if (option & USE_SPECULAR)
			{
				macro.push_back({ "USE_SPECULAR", "1" });
			}
			if (option & USE_METALLIC)
			{
				macro.push_back({ "USE_METALLIC", "1" });
			}
			if (option & USE_ROUGHNESS)
			{
				macro.push_back({ "USE_ROUGHNESS", "1" });
			}
		    if (option & USE_A0)
			{
				macro.push_back({ "USE_A0", "1" });
			}
			if (option & USE_CUSTOM0)
			{
				macro.push_back({ "USE_CUSTOM0", "1" });
			}
			if (option & USE_CUSTOM1)
			{
				macro.push_back({ "USE_CUSTOM1", "1" });
			}
			macro.push_back({ nullptr, nullptr });
			device->CreateShader(m_ShaderDirPath + L"Graphics/" + L"GBufferVS.hlsl", &VS, macro);
			device->CreateShader(m_ShaderDirPath + L"Graphics/" + L"GBufferPS.hlsl", &PS, macro);


			pso.SetRenderTargetFormats({ 0 }, { DXGI_FORMAT_R8G8B8A8_UNORM }, DXGI_FORMAT_D24_UNORM_S8_UINT);
			pso.BindVertexShader(VS);
			pso.BindPixelShader(PS);
			pso.Finalize();
		}

		void PSOCache::CreateSkyPSO(GraphicsPSO& pso, PSOCache::EGraphicsMacroOption option)
		{
			VertexShader VS;
			PixelShader PS;
			auto device = GraphicsDevice::GetPointer();

			
			device->CreateShader(m_ShaderDirPath + L"Graphics/" + L"SkyBoxVS.hlsl", &VS);
			device->CreateShader(m_ShaderDirPath + L"Graphics/" + L"SkyBoxPS.hlsl", &PS);

			pso.SetDepthStencilState(DepthStencilState::LessEqual());
			pso.SetBlendState(BlendState::Default());
			pso.SetRasterizerState(RasterizerState::CullNone());
			pso.SetRenderTargetFormats({ 0 }, { DXGI_FORMAT_R8G8B8A8_UNORM }, DXGI_FORMAT_D24_UNORM_S8_UINT);
			pso.BindVertexShader(VS);
			pso.BindPixelShader(PS);



			D3D12_INPUT_ELEMENT_DESC input_element[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			};
			pso.SetInputLayout(1, input_element);
			pso.Finalize();

		}
		// Compute
		void PSOCache::CreateEquirect2Cube(ComputePSO& pso, EComputeMacroOption option)
		{
			DefaultBindComputeShader(pso, L"Equirect2CubeCS.hlsl");
		}
		void PSOCache::CreateGenerateMipMaps(ComputePSO& pso, EComputeMacroOption option)
		{
			DefaultBindComputeShader(pso, L"GenerateMipsCS.hlsl");
		}
		void PSOCache::CreateIrradianceMap(ComputePSO& pso, EComputeMacroOption option)
		{
			DefaultBindComputeShader(pso, L"IrrmapCS.hlsl");
		}
		void PSOCache::CreateSpecularBRDF(ComputePSO& pso, EComputeMacroOption option)
		{
			DefaultBindComputeShader(pso, L"SpecularBrdfCS.hlsl");
		}
		void PSOCache::CreateSpecularMap(ComputePSO& pso, EComputeMacroOption option)
		{
			DefaultBindComputeShader(pso, L"Specularmap.hlsl");
		}
		void PSOCache::DefaultBindComputeShader(ComputePSO& pso, const std::wstring& shadername)
		{
			ComputeShader CS;
			auto device = GraphicsDevice::GetPointer();
			device->CreateShader(m_ShaderDirPath + L"Compute/" + shadername, &CS);
			pso.BindComputeShader(CS);
			pso.Finalize();
		}
	}
}