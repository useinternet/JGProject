#include"pch.h"
#include"Renderer.h"
#include"GraphicsDevice.h"
#include"VertexCollection.h"
#include"Commander.h"
#include"Mesh.h"
#include"Camera.h"
#include"RenderObject.h"
#include"RootSignatureCache.h"
#include"GeometryGenerator.h"
#include"LightingPass.h"
#include"GBuffer.h"
#include"ToneMapping.h"
#include"IBLBaker.h"
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
			
			m_GBuffer = std::make_shared<class GBuffer>();
			m_GBuffer->Init(width, height);
			m_LightingPass = make_shared<LightingPass>();
			m_LightingPass->Init(width, height);

			m_LightingPass->BindResource(ELightingPassSlot::Albedo, *m_GBuffer->GetTexture(EGBufferSlot::Albedo));
			m_LightingPass->BindResource(ELightingPassSlot::Normal, *m_GBuffer->GetTexture(EGBufferSlot::Normal));
			m_LightingPass->BindResource(ELightingPassSlot::Ambient, *m_GBuffer->GetTexture(EGBufferSlot::Ambient));
			m_LightingPass->BindResource(ELightingPassSlot::MaterialProperty, *m_GBuffer->GetTexture(EGBufferSlot::MaterialProperty));
			m_LightingPass->BindResource(ELightingPassSlot::Depth, *m_GBuffer->GetTexture(EGBufferSlot::Depth));



			m_ToneMapping = std::make_shared<ToneMapping>();
			m_ToneMapping->Init(width, height);
			
			m_IBLBaker = std::make_shared<IBLBaker>();
			m_IBLBaker->Init();

			{
				m_Camera = std::make_unique<Camera>();
				m_Camera->SetLens(0.25f * DirectX::XM_PI, (float)width, (float)height, 1.0f, 100000.0f);
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
				m_PassCB.Set(*m_Camera);
			}
			m_GraphicsCommander = m_GraphcisDevice->GetGraphicsCommander();
		}
		void Renderer::RenderEnd()
		{
			m_GraphcisDevice->PushCommander(m_GraphicsCommander);
			m_GraphcisDevice->ExecuteCommander(false);
		}
		void Renderer::PushObject(RenderObject* obj)
		{
			m_GBuffer->PushObject(obj);
		}
		void Renderer::PushObjects(const std::vector<RenderObject*>& objs)
		{
			for (auto& obj : objs)
			{
				m_GBuffer->PushObject(obj);
			}
		}
		void Renderer::GBuffer()
		{
			m_GBuffer->Execute(m_GraphicsCommander, m_PassCB);
		}
		void Renderer::BindSkyTexture(const Texture& texture)
		{
			if (m_SkyTexture.GetResource() == texture.GetResource())
				return;
			m_SkyTexture = texture;
			m_LightingPass->BindSkyTexture(m_SkyTexture);
		}
		void Renderer::Lighting()
		{
			m_LightingPass->Execute(m_GraphicsCommander, m_PassCB);
		}
		void Renderer::PostProcess()
		{
			m_ToneMapping->BindTexture(*m_LightingPass->GetTexture());
			m_ToneMapping->Execute(m_GraphicsCommander, m_PassCB);
		}
		GBuffer* Renderer::GetGBuffer()
		{
			return m_GBuffer.get();
		}
		LightingPass* Renderer::GetLightingPass()
		{
			return m_LightingPass.get();
		}
		ToneMapping*  Renderer::GetToneMapping()
		{
			return m_ToneMapping.get();
		}
		Texture* Renderer::GetTexture()
		{
			return m_ToneMapping->GetTexture();
		}
		void Renderer::BakeIBLTexture(ComputeCommander* commander, const Texture& inTexture, Texture& outSpMap, Texture& outSpbrdf, Texture& outirrMap)
		{
			m_IBLBaker->Bake(commander, inTexture, &outSpbrdf, &outSpMap, &outirrMap);
		}


	}
}