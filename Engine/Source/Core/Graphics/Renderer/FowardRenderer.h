#pragma once
#include "Graphics/Renderer.h"



namespace JG
{
	class IReadBackBuffer;
	class IRootSignature;
	class IRootSignatureCreater;
	class FowardRenderer : public Renderer
	{
		List<SharedPtr<ITexture>> mTargetTextures;
		List<SharedPtr<ITexture>> mTargetDepthTextures;
		List<SharedPtr<IStructuredBuffer>> mExposureSB;
		SharedPtr<IRootSignature> mRootSignature;


		JVector2 mPrevResolution;
		Color    mPrevClearColor;





		RP_Global_Float mExposure;
		RP_Global_Float mInitialMinLog;
		RP_Global_Float mInitialMaxLog;
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
		void InitTextures(const JVector2& size, const Color& clearColor);
		void InitProcesses();
		void InitGlobalRenderParams();
	};
}