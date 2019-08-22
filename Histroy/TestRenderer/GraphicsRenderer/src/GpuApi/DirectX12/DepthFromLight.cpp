#include"pch.h"
#include"DepthFromLight.h"
#include"GraphicsDevice.h"
#include"Light.h"
#include"RenderObject.h"
#include"RootSignatureCache.h"
#include"Commander.h"
#include"Mesh.h"
#include"Renderer.h"
#include"VertexCollection.h"
#include"Material.h"
namespace GR
{
	namespace Dx12
	{
		std::wstring DepthFromLight::m_ShaderVSPath = L"GBufferVS.hlsl";
		std::wstring DepthFromLight::m_ShaderPSPath = L"DepthPS.hlsl";
		void DepthFromLight::SetShaderVSPath(const std::wstring& path)
		{
			m_ShaderVSPath = path;
		}
		void DepthFromLight::SetShaderPSPath(const std::wstring& path)
		{
			m_ShaderPSPath = path;
		}
		void DepthFromLight::Init(uint32_t width, uint32_t height)
		{
			auto device = GraphicsDevice::GetPointer();
			Light::DepthSize = width;

			m_RootSignature = device->GetRootSignatureFromCache(ERootSignature::G_Common);
			{
				m_Viewport.Set((float)width, (float)width);
				m_ScissorRect.Set(width, width);
			}
			{
				VertexShader VS;
				PixelShader  PS;
				auto dir = device->GetGraphicsShaderDirPath();
				device->CreateShader(dir + m_ShaderVSPath, &VS);
				device->CreateShader(dir + m_ShaderPSPath, &PS);

				m_VertexPSO.BindRootSignature(m_RootSignature.GetD3DRootSignature());
				m_VertexPSO.BindPixelShader(PS);
				m_VertexPSO.BindVertexShader(VS);
				m_VertexPSO.SetRenderTargetFormats({ 0 }, { DXGI_FORMAT_R32_FLOAT }, DXGI_FORMAT_D24_UNORM_S8_UINT);
				m_VertexPSO.SetInputLayout(Vertex::GetNumInputLayout(), Vertex::GetInputLayoutDesc());
				m_VertexPSO.Finalize();
			}
			{
				VertexShader VS;
				PixelShader  PS;
				auto dir = device->GetGraphicsShaderDirPath();
				
				device->CreateShader(dir + m_ShaderVSPath, &VS, 
					{ { USE_SKELETAL, MACRO_DEFINE } , {nullptr, nullptr} });
				device->CreateShader(dir + m_ShaderPSPath, &PS);
				m_SkinnedPSO.BindRootSignature(m_RootSignature.GetD3DRootSignature());
				m_SkinnedPSO.BindPixelShader(PS);
				m_SkinnedPSO.BindVertexShader(VS);
				m_SkinnedPSO.SetRenderTargetFormats({ 0 }, { DXGI_FORMAT_R32_FLOAT }, DXGI_FORMAT_D24_UNORM_S8_UINT);
				m_SkinnedPSO.SetInputLayout(SkinnedVertex::GetNumInputLayout(), SkinnedVertex::GetInputLayoutDesc());
				m_SkinnedPSO.Finalize();
			}

		}
		void DepthFromLight::Resize(uint32_t width, uint32_t height)
		{
			assert(false && "not imp resize");
		}
		void DepthFromLight::Execute(GraphicsCommander* commander, const PassCB& passcb)
		{
			auto device = GraphicsDevice::GetPointer();
		
			commander->SetViewport(m_Viewport);
			commander->SetScissorRect(m_ScissorRect);
			commander->SetRootSignature(m_RootSignature);
			while (!m_LightQueue.empty())
			{
				auto light = m_LightQueue.front();  	m_LightQueue.pop();
				auto lightType = light->m_Data.Type;


				switch (lightType)
				{
				case LightCB::Directional:
				case LightCB::Spot:
					Execute_Spot_Directional(commander, light);
					break;
				case LightCB::Point:
					Execute_Point(commander, light);
					break;
				}


			}
		}
		void DepthFromLight::PushLight(Light* light)
		{
			m_LightQueue.push(light);
		}
		void DepthFromLight::PushObject(RenderObject* obj)
		{
			m_ObjectQueue.push(obj);
		}
		void DepthFromLight::Execute_Spot_Directional(GraphicsCommander* commander, Light* light)
		{
			// 렌더 타겟 셋팅
			commander->ClearColor(*light->GetTexture());
			commander->ClearDepthStencil(*light->GetDepthTexture());

			auto rtvs = light->GetRenderTarget()->GetRTVs();
			auto dsv = light->GetRenderTarget()->GetDSV();
			commander->SetRenderTargets((uint32_t)rtvs.size(), rtvs.data(), dsv);

			// 라이트 별 PassCB..
			PassCB pcb = light->GetPassCB();
			commander->SetDynamicConstantBuffer(G_Common_RootParam_PassCB, sizeof(PassCB), &pcb);

			while (!m_ObjectQueue.empty())
			{
				// 머터리얼은 바인딩 nono
				auto obj = m_ObjectQueue.front(); m_ObjectQueue.pop();

				// obj가 스켈레탈 메시라면
				//if(obj->m_Mesh == 스켈레탈 메시)
				// else{
				commander->SetPipelineState(m_VertexPSO);

				if (!obj->m_ObjectCBs.empty())
				{
					commander->SetDynamicStructuredBuffer(G_Common_RootParam_ObjectCB, (uint32_t)obj->m_ObjectCBs.size(), sizeof(ObjectCB),
						obj->m_ObjectCBs.data());
				}
				if (obj->GetMesh())
				{
					obj->GetMesh()->Draw(commander, obj->m_ObjectCBs.size());
				}

			}
		}
		void DepthFromLight::Execute_Point(GraphicsCommander* commander, Light* light)
		{
			ColorTexture* texture = light->GetTexture();
			DepthTexture* depthTexture = light->GetDepthTexture();
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Texture2DArray.ArraySize = 1;
			rtvDesc.Texture2DArray.FirstArraySlice = 0;
			rtvDesc.Texture2DArray.MipSlice = 0;
			rtvDesc.Texture2DArray.PlaneSlice = 0;
			rtvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;

			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.ArraySize = 1;
			dsvDesc.Texture2DArray.FirstArraySlice = 0;
			dsvDesc.Texture2DArray.MipSlice = 0;

			std::queue<RenderObject*> tempQueue = m_ObjectQueue;
			for (int i = 0; i < 6; ++i)
			{
				rtvDesc.Texture2DArray.FirstArraySlice = i;
				dsvDesc.Texture2DArray.FirstArraySlice = i;

				texture->SetRTVDesc(&rtvDesc);
				depthTexture->SetDSVDesc(&dsvDesc);


				commander->ClearColor(*texture);
				commander->ClearDepthStencil(*depthTexture);


				commander->SetRenderTargets(1, &texture->GetRTV(), depthTexture->GetDSV());

				PassCB pcb = light->GetPassCB(i);
				commander->SetDynamicConstantBuffer(G_Common_RootParam_PassCB, sizeof(PassCB), &pcb);

				m_ObjectQueue = tempQueue;
				// 각 카메라 프리스텀 체크(아직 구현 안함)
				while (!m_ObjectQueue.empty())
				{
					// 머터리얼은 바인딩 nono
					auto obj = m_ObjectQueue.front(); m_ObjectQueue.pop();

					// obj가 스켈레탈 메시라면
					//if(obj->m_Mesh == 스켈레탈 메시)
					// else{
					commander->SetPipelineState(m_VertexPSO);

					if (!obj->m_ObjectCBs.empty())
					{
						commander->SetDynamicStructuredBuffer(G_Common_RootParam_ObjectCB, (uint32_t)obj->m_ObjectCBs.size(), sizeof(ObjectCB),
							obj->m_ObjectCBs.data());
					}
					if (obj->GetMesh())
					{
						obj->GetMesh()->Draw(commander, obj->m_ObjectCBs.size());
					}

				}
				
			}

		}
	
	}
}