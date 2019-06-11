#include"pch.h"
#include"Renderer.h"
#include"GraphicsDevice.h"
#include"VertexCollection.h"
#include"Commander.h"
#include"Mesh.h"
#include"Camera.h"
#include"RenderObject.h"
#include"PSOCache.h"
#include"RootSignatureCache.h"
#include"GeometryGenerator.h"
using namespace std;
namespace GR
{
	namespace Dx12
	{
		Renderer::Renderer()
		{}


		GraphicsDevice* Renderer::GetDevice()
		{
			return m_GraphcisDevice;
		}
		const GraphicsDevice* Renderer::GetDevice() const
		{
			return m_GraphcisDevice;
		}

		void Renderer::Initialize(uint32_t width, uint32_t height)
		{
			for (uint32_t i = GBUFFER; i < NUMPASS; ++i)
			{
				m_PassCommanders[(ERenderPass)i] = nullptr;
			}
			m_GCommonRootSignature = m_GraphcisDevice->GetRootSignatureFromCache(ERootSignature::G_Common);
			GBufferInit(width, height);
			ToneMappingInit(width, height);
			{
				m_Camera = std::make_unique<Camera>();
				m_Camera->SetLens(0.25f * DirectX::XM_PI, (float)width, (float)height, 1.0f, 100000.0f);

				// 스카이 박스
				m_SkyBox = make_shared<RenderObject>();
				ObjectCB objectcb;
				DirectX::XMStoreFloat4x4(&objectcb.World, DirectX::XMMatrixIdentity());
				m_SkyBox->AddInstance(objectcb);
				auto data = GeometryGenerator::CreateBox(100000, 100000, 100000, 0);
				Mesh skyMesh;
				skyMesh.CreateMesh(m_GraphcisDevice->GetLoadCommander(GraphicsDevice::MESH_LOADER),
					data.Vertices, data.Indices);
				m_SkyBox->BindMesh(&skyMesh);
			}


		}
		void Renderer::Resize(uint32_t width, uint32_t height)
		{
			assert(false && "resize not imp");
		}
		Camera* Renderer::GetCamera()
		{
			return m_Camera.get();
		}
		void Renderer::RenderBegin()
		{
	
			m_GraphcisDevice->Flush();
			if (m_Camera->IsUpadte())
			{
				m_GBufferPass.passCB.Set(*m_Camera);
				m_ToneMappingPass.passCB = m_GBufferPass.passCB;
			}
		}
		void Renderer::RenderEnd()
		{
			m_GraphcisDevice->ExecuteCommander(false);
		}
		void Renderer::GBufferOn()
		{
			m_CurrentRenderPass = GBUFFER;
			m_PassCommanders[GBUFFER] = m_GraphcisDevice->GetGraphicsCommander();
			auto commander = m_PassCommanders[GBUFFER];
			commander->SetViewport(m_GBufferPass.viewPort);
			commander->SetScissorRect(m_GBufferPass.scissorRect);


			commander->ClearColor(*m_GBufferPass.RT.GetTexture(RenderTarget::Slot0));
			commander->ClearDepthStencil(*m_GBufferPass.RT.GetDepthTexture());
			commander->SetRenderTarget(*m_GBufferPass.RT.GetRTVs().data(), m_GBufferPass.RT.GetDSV());
			commander->SetRootSignature(m_GCommonRootSignature);

			commander->SetDynamicConstantBuffer(G_Common_RootParam_PassCB, sizeof(PassCB), &m_GBufferPass.passCB);
		}
		void Renderer::GBufferRender(const RenderObject& obj)
		{
			auto commander = m_PassCommanders[GBUFFER];

			if (commander == nullptr)
				return;

			// PiplineState 옵션 체크
			{
				int option = PSOCache::USE_NULL;
				GraphicsPSO pso;
				for (uint32_t i = 0; i < RenderObject::NUM_TEXTURESLOT; ++i)
				{
					if (obj.m_Textures[i].IsValid())
					{
						switch (i)
						{
						case RenderObject::ALBEDO:
							option |= PSOCache::USE_ALBEDO;
							break;
						case RenderObject::NORMAL:
							option |= PSOCache::USE_NORMAL;
							break;
						case RenderObject::SPECULAR:
							option |= PSOCache::USE_SPECULAR;
							break;
						case RenderObject::METALLIC:
							option |= PSOCache::USE_METALLIC;
							break;
						case RenderObject::ROUGHNESS:
							option |= PSOCache::USE_ROUGHNESS;
							break;
						case RenderObject::A0:
							option |= PSOCache::USE_A0;
							break;
						case RenderObject::CUSTOM0:
							option |= PSOCache::USE_CUSTOM0;
							break;
						case RenderObject::CUSTOM1:
							option |= PSOCache::USE_CUSTOM1;
							break;
						}
					}
				}



				pso = m_GraphcisDevice->GetGraphicsPSOFromCache(m_GCommonRootSignature, PSOCache::GBUFFER, (PSOCache::EGraphicsMacroOption)option);
				commander->SetPipelineState(pso);
			}
			// 텍스쳐 바인딩
			{
				auto srv_Vec = obj.GetTextureSRV();
				commander->SetSRVDescriptorTable(G_Common_RootParam_Texture, (uint32_t)srv_Vec.size(), srv_Vec.data());
			}
			// 큐브 텍스쳐 바인딩
			{
				auto srv_Vec = obj.GetCubeTextureSRV();
				commander->SetSRVDescriptorTable(G_Common_RootParam_CubeTexture, (uint32_t)srv_Vec.size(), srv_Vec.data());
			}


			
			// 오브젝트 데이터 바인딩 및 메시 렌더링
			{
				if (!obj.m_ObjectCBs.empty())
				{
					commander->SetDynamicStructuredBuffer(G_Common_RootParam_ObjectCB, (uint32_t)obj.m_ObjectCBs.size(), sizeof(ObjectCB),
						obj.m_ObjectCBs.data());
				}
				if (obj.m_Mesh)
				{
					obj.m_Mesh->Draw(commander, (uint32_t)obj.m_ObjectCBs.size());
				}
			}

			
		}
		void Renderer::GBufferOff()
		{
			if (m_PassCommanders[GBUFFER] == nullptr)
				return;
			m_GraphcisDevice->PushCommander(m_PassCommanders[GBUFFER]);
		}
		ColorTexture* Renderer::GetTexture()
		{
			switch (m_CurrentRenderPass)
			{
			case GBUFFER:
				return m_GBufferPass.RT.GetTexture(RenderTarget::Slot0);
			case TONEMAPPING:
				return m_ToneMappingPass.RT.GetTexture(RenderTarget::Slot0);
			}
			return nullptr;
		}
		RenderTarget* Renderer::GetRenderTarget(ERenderPass pass)
		{
			switch (pass)
			{
			case GBUFFER:
				return &m_GBufferPass.RT;
			case TONEMAPPING:
				return &m_ToneMappingPass.RT;
			}
			return nullptr;
		}
		void Renderer::ToneMapping()
		{
			m_CurrentRenderPass = TONEMAPPING;
			m_PassCommanders[TONEMAPPING] = m_GraphcisDevice->GetGraphicsCommander();
			auto sceneTexture = m_GBufferPass.RT.GetTexture(RenderTarget::Slot0);
			sceneTexture->SetSRVDesc(nullptr);
			sceneTexture->SetRTVDesc(nullptr);
			auto commander = m_PassCommanders[TONEMAPPING];


			commander->SetViewport(m_ToneMappingPass.viewPort);
			commander->SetScissorRect(m_ToneMappingPass.scissorRect);
			commander->ClearColor(*m_ToneMappingPass.RT.GetTexture(RenderTarget::Slot0));
			commander->SetRenderTarget(m_ToneMappingPass.RT.GetTexture(RenderTarget::Slot0)->GetRTV());
			commander->SetRootSignature(m_GCommonRootSignature);
			commander->SetDynamicConstantBuffer(G_Common_RootParam_PassCB, sizeof(PassCB), &m_ToneMappingPass.passCB);
			auto pso = m_GraphcisDevice->GetGraphicsPSOFromCache(m_GCommonRootSignature, PSOCache::TONEMAPPING, PSOCache::USE_NULL);
			commander->SetPipelineState(pso);
			commander->SetSRVDescriptorTable(G_Common_RootParam_Texture, 1, &sceneTexture->GetSRV());
			commander->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			commander->Draw(6);

			m_GraphcisDevice->PushCommander(commander);
			m_PassCommanders[TONEMAPPING] = nullptr;
		}
		void Renderer::SkyBoxRender(const Texture& texture)
		{
			// 임시로 GBuffer에서 렌더링
			auto commander = m_PassCommanders[GBUFFER];

			if (commander == nullptr)
				return;



			auto pso = m_GraphcisDevice->GetGraphicsPSOFromCache(m_GCommonRootSignature,
				PSOCache::SKYBOX, PSOCache::USE_NULL);
			commander->SetPipelineState(pso);
	
			if (m_SkyBox->m_CubeTextures[RenderObject::SLOT1].GetResource() !=  texture.GetResource())
			{
				m_SkyBox->BindCubeTexture(RenderObject::SLOT1, texture);
			}
			auto cube_srv_Vec = m_SkyBox->GetCubeTextureSRV();
			commander->SetSRVDescriptorTable(G_Common_RootParam_CubeTexture,
				(uint32_t)cube_srv_Vec.size(), cube_srv_Vec.data());

			commander->SetDynamicStructuredBuffer(G_Common_RootParam_ObjectCB,
				(uint32_t)m_SkyBox->m_ObjectCBs.size(), sizeof(ObjectCB), m_SkyBox->m_ObjectCBs.data());

			m_SkyBox->m_Mesh->Draw(commander);

		}
		void Renderer::GBufferInit(uint32_t width, uint32_t height)
		{
			// 리소스 정의
			{
		
				GPUResource gpuresource = m_GraphcisDevice->CreateRenderTargetGPUResource(
					DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1);


				auto sceneTexture = m_GraphcisDevice->CreateColorTexture(gpuresource);


				GPUResource depthresource = m_GraphcisDevice->CreateDepthStencilGPUResource(
					DXGI_FORMAT_D24_UNORM_S8_UINT, width, height, 1);

				auto depthTexture = m_GraphcisDevice->CreateDepthTexture(depthresource);

				m_GBufferPass.RT.AttachTexture(RenderTarget::Slot0, sceneTexture);
				m_GBufferPass.RT.AttachTexture(RenderTarget::DepthStencil, depthTexture);
			}
			{
				m_GBufferPass.viewPort.Set((float)width, (float)height);
				m_GBufferPass.scissorRect.Set(width, height);
			}
		}
		void Renderer::ToneMappingInit(uint32_t width, uint32_t height)
		{
			GPUResource gpuresource = m_GraphcisDevice->CreateRenderTargetGPUResource(
				DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1);
			auto sceneTexture = m_GraphcisDevice->CreateColorTexture(gpuresource);


			m_ToneMappingPass.RT.AttachTexture(RenderTarget::Slot0, sceneTexture);
			m_ToneMappingPass.viewPort.Set((float)width, (float)height);
			m_ToneMappingPass.scissorRect.Set(width, height);
		}
		void Renderer::BakeIBLTexture(ComputeCommander* commander, const Texture& inTexture, Texture& outSpMap, Texture& outSpbrdf, Texture& outirrMap)
		{
			Texture texture = inTexture;
			if (texture->GetDesc().DepthOrArraySize != 6)
			{
				// 로그
				return;
			}
			// Srv Desc 작성
			auto resourceDesc = texture->GetDesc();
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = resourceDesc.Format;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MipLevels = resourceDesc.MipLevels;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			texture.SetSRVDesc(&srvDesc);

			
			// 리소스 생성
			{
				auto desc = resourceDesc;
				desc.Width = 1024; desc.Height = 1024;
				desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
				GPUResource spmapResource = m_GraphcisDevice->CreateGPUResource(desc, nullptr,
					GraphicsDevice::DEFAULT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				outSpMap = m_GraphcisDevice->CreateTexture(spmapResource);

				//

				desc.Width = 32; desc.Height = 32;
				desc.DepthOrArraySize = 6;
				desc.MipLevels = 1;

				GPUResource irrResource = m_GraphcisDevice->CreateGPUResource(desc, nullptr,
					GraphicsDevice::DEFAULT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				outirrMap = m_GraphcisDevice->CreateTexture(irrResource);

				//
				desc.Width = 256; desc.Height = 256;
				desc.DepthOrArraySize = 1;
				desc.Format = DXGI_FORMAT_R16G16_FLOAT;
				GPUResource spbrdfResource = m_GraphcisDevice->CreateGPUResource(desc, nullptr,
					GraphicsDevice::DEFAULT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				outSpbrdf = m_GraphcisDevice->CreateTexture(spbrdfResource);

			}


			// UAV 작성
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = resourceDesc.Format;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.Texture2DArray.ArraySize = 6;
			uavDesc.Texture2DArray.FirstArraySlice = 0;
			uavDesc.Texture2DArray.MipSlice = 0;
			uavDesc.Texture2DArray.PlaneSlice = 0;



			// 루트 서명 및 PSO
			
			

			for (UINT arraySlice = 0; arraySlice < 6; ++arraySlice)
			{
				commander->CopyResourceRegion(outSpMap, 0, 0, 0, 0, texture, 0, arraySlice);
			}

			commander->TransitionBarrier(outSpMap, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);

			

			// prefiltered specular Map
			{
				RootSignature rootSig = m_GraphcisDevice->GetRootSignatureFromCache(ERootSignature::C_SpecularMap);
				ComputePSO pso = m_GraphcisDevice->GetComputePSOFromCache(rootSig, PSOCache::SPECULARMAP, PSOCache::USE_NULL);

				commander->SetRootSignature(rootSig);
				commander->SetPipelineState(pso);
				commander->SetSRVDescriptorTable(EC_SpecularMap_RootParam_InputTexture, 1, &texture.GetSRV());

				const float deltaRoughness = 1.0f / std::max(float(resourceDesc.MipLevels - 1), 1.0f);

				for (uint32_t level = 1, size = 512; level < resourceDesc.MipLevels; ++level, size /= 2)
				{
					uavDesc.Texture2DArray.MipSlice = level;
					outSpMap.SetUAVDesc(&uavDesc);
					const UINT numGroups = std::max<UINT>(1, size / 32);
					const float spmapRoughness = level * deltaRoughness;

					float constant_roughness = spmapRoughness;


					commander->SetDynamicConstantBuffer(EC_SpecularMap_RootParam_Roughness, sizeof(float), &constant_roughness);
					commander->SetUAVDescriptorTable(EC_SpecularMap_RootParam_OutputTexture, 1, &outSpMap.GetUAV());
					commander->Dispatch(numGroups, numGroups, 6);
				
				}
				outSpMap.SetUAVDesc(nullptr);

				commander->TransitionBarrier(outSpMap, D3D12_RESOURCE_STATE_COMMON, true);
			}

			// Compute diffuse irradiance cubemap
			{
				uavDesc.Texture2DArray.MipSlice = 0;
				outirrMap.SetUAVDesc(&uavDesc);


				RootSignature rootSig = m_GraphcisDevice->GetRootSignatureFromCache(ERootSignature::C_IrrMap);
				ComputePSO pso = m_GraphcisDevice->GetComputePSOFromCache(rootSig, PSOCache::IRRADIANCEMAP, PSOCache::USE_NULL);
				commander->SetRootSignature(rootSig);
				commander->SetPipelineState(pso);

		
				commander->SetSRVDescriptorTable(EC_IrrMap_RootParam_InputTexture, 1, &texture.GetSRV());
				commander->SetUAVDescriptorTable(EC_IrrMap_RootParam_OutputTexture, 1, &outirrMap.GetUAV());
				commander->Dispatch(outirrMap->GetDesc().Width / 32, outirrMap->GetDesc().Height / 32, 6);


				commander->UAVBarrier(outirrMap, true);

				outirrMap.SetUAVDesc(nullptr);
			}

			// Compute Cook-Torrance BRDF 2D LUT for split-sum approximation
			{
				RootSignature rootSig = m_GraphcisDevice->GetRootSignatureFromCache(ERootSignature::C_SpecularBRDF);
				ComputePSO pso = m_GraphcisDevice->GetComputePSOFromCache(rootSig, PSOCache::SPECULARBRDF, PSOCache::USE_NULL);


				commander->SetRootSignature(rootSig);
				commander->SetPipelineState(pso);

				commander->SetUAVDescriptorTable(EC_SpecularBRDF_RootParam_OutputTexture, 1, &outSpbrdf.GetUAV());
				commander->Dispatch(outSpbrdf->GetDesc().Width / 32, outSpbrdf->GetDesc().Height / 32, 1);
				commander->UAVBarrier(outSpbrdf, true);
			}

		}


	}
}