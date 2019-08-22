#include"pch.h"
#include"GBuffer.h"
#include"GraphicsDevice.h"
#include"Commander.h"
#include"RootSignatureCache.h"
#include"RenderObject.h"
#include"GraphicsResource.h"
#include"Mesh.h"
#include"Material.h"
#include"Camera.h"
#include"Renderer.h"
using namespace std;

namespace GR
{
	namespace Dx12
	{
		DXGI_FORMAT GBuffer::ms_GBufferFormats[(int)EGBufferSlot::Num_Slot] = {
			DXGI_FORMAT_R8G8B8A8_UNORM,  // albedo
			DXGI_FORMAT_R8G8B8A8_UNORM,  // normal
			DXGI_FORMAT_R8G8B8A8_UNORM,  // ambient
			DXGI_FORMAT_R8G8B8A8_UNORM,  // metallic, rougness
			DXGI_FORMAT_R32_FLOAT,       // lineardepth
			DXGI_FORMAT_D24_UNORM_S8_UINT// depthstencil
		};


		void GBuffer::Init(uint32_t width, uint32_t height)
		{
			auto device = GraphicsDevice::GetPointer();
			m_Width  = width;
			m_Height = height;
			// 리소스 생성 후 렌더타겟에 바인딩
			{
				for (uint32_t i = 0; i < (uint32_t)EGBufferSlot::DepthStencil; ++i)
				{
					float4 clearColor = { 0.0f,0.0f,0.0f,0.0f };
					if (i == (uint32_t)EGBufferSlot::Depth)
						clearColor.x = 1.0f;
					GPUResource resource = device->CreateRenderTargetGPUResource(
						ms_GBufferFormats[i], width, height, 1, 1,  clearColor);

					ColorTexture texture = device->CreateColorTexture(resource, clearColor);

					m_Rendertarget.AttachTexture((RenderTarget::ESlot)i, texture);
				}
				GPUResource depthresource = device->CreateDepthStencilGPUResource(
					ms_GBufferFormats[(int)EGBufferSlot::DepthStencil], width, height, 1);
				DepthTexture depthtexture = device->CreateDepthTexture(depthresource);

				m_Rendertarget.AttachTexture(RenderTarget::DepthStencil, depthtexture);
			}

			// Viewport 및 scissorRect 설정
			{
				m_Viewport.Set((float)width, (float)height);
				m_ScissorRect.Set(width, height);
			}
			m_RootSignature = device->GetRootSignatureFromCache(ERootSignature::G_Common);
		}
		void GBuffer::Resize(uint32_t width, uint32_t height)
		{
			assert(false && "not implement gbuffer resize");
		}
		void GBuffer::PushObject(RenderObject* obj)
		{
			m_ObjQueue.push(obj);
		}
		void GBuffer::Execute(GraphicsCommander* commander, const PassCB& passcb)
		{
			auto device = GraphicsDevice::GetPointer();

			commander->SetViewport(m_Viewport);
			commander->SetScissorRect(m_ScissorRect);
			for (uint32_t i = 0; i < (uint32_t)EGBufferSlot::DepthStencil; ++i)
			{
				commander->ClearColor(*m_Rendertarget.GetTexture((RenderTarget::ESlot)i));
			}
			commander->ClearDepthStencil(*m_Rendertarget.GetDepthTexture());

			auto rtvs = m_Rendertarget.GetRTVs();
			commander->SetRenderTargets((uint32_t)rtvs.size(), rtvs.data(), m_Rendertarget.GetDSV());
			commander->SetRootSignature(m_RootSignature);
			commander->SetDynamicConstantBuffer(G_Common_RootParam_PassCB, sizeof(PassCB),
				&passcb);

			while (!m_ObjQueue.empty())
			{
				auto obj = m_ObjQueue.front();
				m_ObjQueue.pop();
				// 머터리얼 바인딩
				{
					if (obj->m_Material)
					{
						obj->m_Material->BindResourceAtGPU(commander);
					}

				}

				// 오브젝트 데이터 바인딩 및 메시 렌더링
				{
					std::vector<ObjectCB> datas;
					if (!obj->m_ObjectCBs.empty())
					{
						//for (int i = 0; i < obj->m_ObjectCBs.size(); ++i)
						//{
						//	auto boundingbox = obj->m_BoundingBoxs[i];
						//	auto objcb = obj->m_ObjectCBs[i];

						//	DirectX::XMStoreFloat3(&boundingbox.Center, DirectX::XMVector3TransformCoord(
						//		DirectX::XMLoadFloat3(&boundingbox.Center),
						//		DirectX::XMLoadFloat4x4(&objcb.World)));

						//	if (GetRenderer()->GetCamera()->GetFrustum().Contains(boundingbox) != DirectX::DISJOINT)
						//	{
						//		datas.push_back(obj->m_ObjectCBs[i]);
						//	}
						//}
						datas = obj->m_ObjectCBs;
						commander->SetDynamicStructuredBuffer(G_Common_RootParam_ObjectCB, (uint32_t)datas.size(), sizeof(ObjectCB),
							datas.data());
					}
					if (obj->m_Mesh)
					{
						obj->m_Mesh->Draw(commander, (uint32_t)datas.size());
					}
				}

			}
		}
		Texture* GBuffer::GetTexture(EGBufferSlot slot)
		{
			if (slot == EGBufferSlot::DepthStencil)
				return nullptr;
			return m_Rendertarget.GetTexture((RenderTarget::ESlot)slot);
		}
		DepthTexture* GBuffer::GetDepthStencilTexture()
		{
			return m_Rendertarget.GetDepthTexture();
		}
		GraphicsPSO GBuffer::GetPSO(){
			GraphicsPSO pso;
			auto device = GraphicsDevice::GetPointer();
			auto sig = device->GetRootSignatureFromCache(ERootSignature::G_Common);
			pso.BindRootSignature(sig);

			pso.SetRenderTargetFormats(
				{ // 슬롯
				(unsigned int)EGBufferSlot::Albedo,
				(unsigned int)EGBufferSlot::Normal,
				(unsigned int)EGBufferSlot::Ambient,
				(unsigned int)EGBufferSlot::MaterialProperty,
				(unsigned int)EGBufferSlot::Depth},
				{ // 포맷
				ms_GBufferFormats[(int)EGBufferSlot::Albedo],
				ms_GBufferFormats[(int)EGBufferSlot::Normal],
				ms_GBufferFormats[(int)EGBufferSlot::Ambient],
				ms_GBufferFormats[(int)EGBufferSlot::MaterialProperty],
				ms_GBufferFormats[(int)EGBufferSlot::Depth]},
				ms_GBufferFormats[(int)EGBufferSlot::DepthStencil]);

			return pso;
		}
	}
}