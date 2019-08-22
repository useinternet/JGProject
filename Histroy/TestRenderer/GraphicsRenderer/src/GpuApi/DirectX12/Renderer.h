#pragma once
#include"Dx12Include.h"
#include"GraphicsResource.h"
#include"Viewport.h"
#include"ScissorRect.h"
#include"RootSignature.h"
#include"RenderTarget.h"
#include"PipelineState.h"
#include"ShaderType.h"

namespace GR
{
	namespace Dx12
	{
		// 라이팅 패쓰에 큐브맵 그리기
		class GraphicsDevice;
		class GraphicsCommander;
		class Camera;
		class Mesh;
		class PSOCache;
		class RootSignatureCache;
		class RenderObject;
		class LightingPass;
		class ToneMapping;
		class GBuffer;
		class IBLBaker;
		class DepthFromLight;
		class ShadowMapping;
		class Light;
		/* Renderer 기능에 필요한것

		*/

		class Renderer
		{
			friend GraphicsDevice;
		private:
			GraphicsDevice* m_GraphcisDevice;
			std::shared_ptr<Camera> m_Camera;
			std::shared_ptr<GBuffer> m_GBuffer;
			std::shared_ptr<LightingPass> m_LightingPass;
			std::shared_ptr<ToneMapping>  m_ToneMapping;
			std::shared_ptr<IBLBaker>     m_IBLBaker;
			std::shared_ptr<DepthFromLight> m_DepthFromLight;
			std::shared_ptr<ShadowMapping>  m_ShadowMapping;
			std::queue<RenderObject*>   m_ObjectQue;
			std::queue<Light*>          m_LightQue;
			Texture m_SkyTexture;
			PassCB  m_PassCB;
			float m_Distance;
			float m_PrevDistance;
		public:
			GraphicsCommander* m_GraphicsCommander;
		public:
			Renderer();
			
			GraphicsDevice* GetDevice();
			const GraphicsDevice* GetDevice() const;

			void Initialize(uint32_t width, uint32_t height);
			void Resize(uint32_t width, uint32_t height);
			Camera* GetCamera();
		public:
			void Execute();
		public: // GBuffer
			void PushObject(RenderObject* obj);
			void PushObjects(const std::vector<RenderObject*>& objs);
			void PushLight(Light* light);
			void PushLights(const std::vector<Light*>& lights);
			void BindSkyTexture(const Texture& texture);

			class GBuffer* GetGBuffer();
			LightingPass* GetLightingPass();
			ShadowMapping* GetShadowMapping();
			ToneMapping*  GetToneMapping();
			Texture* GetTexture();
			const PassCB& GetPassCB() const {
				return m_PassCB;
			}
		public:
			void BakeIBLTexture(ComputeCommander* commander, const Texture& inTexture, Texture& outSpMap, Texture& outSpbrdf, Texture& irrMap);
		public:

		};



	}
}