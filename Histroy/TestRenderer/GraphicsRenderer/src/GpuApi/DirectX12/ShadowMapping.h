#pragma once
#include"GraphicsPass.h"




namespace GR
{
	namespace Dx12
	{
		class Light;
		struct ShadowMappingCB
		{
			float4x4 CamInvViewProj;
			float    ShadowSize;
			uint32_t LightCount;
			float2   padding;
		};
		class ShadowMapping : public GraphicsPass
		{
			static std::wstring m_ShaderVSPath;
			static std::wstring m_ShaderPSPath;
			static void SetShaderVSPath(const std::wstring& path);
			static void SetShaderPSPath(const std::wstring& path);
		private:
			RenderTarget  m_Rendertarget;
			RootSignature m_RootSignature;
			Viewport      m_Viewport;
			ScissorRect   m_ScissorRect;
			GraphicsPSO   m_PSO;
			Texture       m_DepthTexture;
			std::queue<Light*> m_LightQue;
		public:
			virtual void Init(uint32_t width, uint32_t height) override;
			virtual void Resize(uint32_t width, uint32_t height) override;
			virtual void Execute(GraphicsCommander* commander, const PassCB& passcb) override;
			void PushLight(Light* l);
			void BindDepthTexture(Texture& texture);
			Texture* GetTexture();
			const Texture* GetTexture() const;
		};
	}
}