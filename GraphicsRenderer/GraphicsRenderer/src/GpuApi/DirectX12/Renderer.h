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
			Texture m_SkyTexture;
			PassCB  m_PassCB;
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
			void RenderBegin();
			void RenderEnd();
		public: // GBuffer
			void PushObject(RenderObject* obj);
			void PushObjects(const std::vector<RenderObject*>& objs);
			void GBuffer();
			void BindSkyTexture(const Texture& texture);
			void Lighting();
			void PostProcess();

			class GBuffer* GetGBuffer();
			LightingPass* GetLightingPass();
			ToneMapping*  GetToneMapping();
			Texture* GetTexture();
		public:
			void BakeIBLTexture(ComputeCommander* commander, const Texture& inTexture, Texture& outSpMap, Texture& outSpbrdf, Texture& irrMap);
		public:

		};



	}
}