#pragma once
#include "Graphics/Renderer.h"



namespace JG
{
	class FowardRenderer : public Renderer
	{
		List<SharedPtr<ITexture>> mTargetTextures;
		List<SharedPtr<ITexture>> mTargetDepthTextures;

		JVector2 mPrevResolution;
		Color    mPrevClearColor;
		bool     mPrevIsHDR = false;
	public:
		FowardRenderer();
		virtual ~FowardRenderer() = default;
	public:
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::Foward; }
		virtual void ReadyImpl(IGraphicsAPI* api, Graphics::RenderPassData* renderPassData, const RenderInfo& info) override;
		virtual void RenderImpl(IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result) override;
		virtual void CompeleteImpl(IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result) override;
		virtual int ArrangeObject(const ObjectInfo& info) override;
	private:
		void InitTexture(const JVector2& size, const Color& clearColor, bool ishdr);
	};
}