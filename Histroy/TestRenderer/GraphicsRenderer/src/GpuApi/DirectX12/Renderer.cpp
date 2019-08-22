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
#include"DepthFromLight.h"
#include"LightingPass.h"
#include"GBuffer.h"
#include"ToneMapping.h"
#include"IBLBaker.h"
#include"Light.h"
#include"ShadowMapping.h"
using namespace std;
namespace GR
{
	namespace Dx12
	{
		Renderer::Renderer()
		{
			m_Distance = 0.0f;
			m_PrevDistance = 0.0f;
			GraphicsPass::ms_Renderer = this;
		}


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
			m_ShadowMapping = std::make_shared<ShadowMapping>();
			m_ShadowMapping->Init(width, height);
			m_ToneMapping = std::make_shared<ToneMapping>();
			m_ToneMapping->Init(width, height);

			m_IBLBaker = std::make_shared<IBLBaker>();
			m_IBLBaker->Init();

			m_DepthFromLight = std::make_shared<DepthFromLight>();
			m_DepthFromLight->Init(1024, 1024);

			m_LightingPass->BindResource(ELightingPassSlot::Albedo, *m_GBuffer->GetTexture(EGBufferSlot::Albedo));
			m_LightingPass->BindResource(ELightingPassSlot::Normal, *m_GBuffer->GetTexture(EGBufferSlot::Normal));
			m_LightingPass->BindResource(ELightingPassSlot::Ambient, *m_GBuffer->GetTexture(EGBufferSlot::Ambient));
			m_LightingPass->BindResource(ELightingPassSlot::MaterialProperty, *m_GBuffer->GetTexture(EGBufferSlot::MaterialProperty));
			m_LightingPass->BindResource(ELightingPassSlot::Depth, *m_GBuffer->GetTexture(EGBufferSlot::Depth));
			m_LightingPass->BindResource(ELightingPassSlot::SahdowMap, *m_ShadowMapping->GetTexture());
			m_ShadowMapping->BindDepthTexture(*m_GBuffer->GetTexture(EGBufferSlot::Depth));





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
		void Renderer::Execute()
		{
	
			m_GraphcisDevice->Flush();
			if (m_Camera->IsUpadte())
			{
				m_PassCB.Set(*m_Camera);
			}
			m_GraphicsCommander = m_GraphcisDevice->GetGraphicsCommander();

			while (!m_ObjectQue.empty())
			{
				m_GBuffer->PushObject(m_ObjectQue.front());
				m_DepthFromLight->PushObject(m_ObjectQue.front());
				m_ObjectQue.pop();
			}
			m_PassCB.LightCount = 0;
			while (!m_LightQue.empty())
			{
			
				Light* l = m_LightQue.front();
				if (l->m_Data.Type == LightCB::Directional && m_PrevDistance != m_Distance)
				{
					DirectionLight* dl = static_cast<DirectionLight*>(l);
					m_PrevDistance = m_Distance;
					dl->m_Distance = m_Distance;
					dl->m_IsUpdate = true;
				}
				m_DepthFromLight->PushLight(l);
				m_ShadowMapping->PushLight(l);
				m_LightingPass->PushLight(l);
				m_LightQue.pop();
				m_PassCB.LightCount++;
			}


			// gbuffer
			m_GBuffer->Execute(m_GraphicsCommander, m_PassCB);


			// depth from light
			m_DepthFromLight->Execute(m_GraphicsCommander, m_PassCB);
			// shadowmapping
			m_ShadowMapping->Execute(m_GraphicsCommander, m_PassCB);

			//// light
			m_LightingPass->Execute(m_GraphicsCommander, m_PassCB);

			//// tonemapping
			//m_ToneMapping->BindTexture(*m_LightingPass->GetTexture());
			//m_ToneMapping->Execute(m_GraphicsCommander, m_PassCB);


			m_GraphcisDevice->PushCommander(m_GraphicsCommander);
			m_GraphcisDevice->ExecuteCommander(false);
		}
		void Renderer::PushObject(RenderObject* obj)
		{
			float3 v = obj->GetMesh()->GetBoundingBox().Extents;


			float max_f = std::max(v.x, std::max(v.y, v.z));
			m_Distance = std::max(m_Distance, max_f);
			m_ObjectQue.push(obj);
		}
		void Renderer::PushObjects(const std::vector<RenderObject*>& objs)
		{
			for (auto& obj : objs)
			{
				PushObject(obj);
			}
		}
		void Renderer::PushLight(Light* light)
		{
			m_LightQue.push(light);
		}
		void Renderer::PushLights(const std::vector<Light*>& lights)
		{
			for (auto& l : lights)
			{
				PushLight(l);
			}
		}
		void Renderer::BindSkyTexture(const Texture& texture)
		{
			if (m_SkyTexture.GetResource() == texture.GetResource())
				return;
			m_SkyTexture = texture;
			m_LightingPass->BindSkyTexture(m_SkyTexture);
		}
		GBuffer* Renderer::GetGBuffer()
		{
			return m_GBuffer.get();
		}
		LightingPass* Renderer::GetLightingPass()
		{
			return m_LightingPass.get();
		}
		ShadowMapping* Renderer::GetShadowMapping() {
			return m_ShadowMapping.get();
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