#pragma once
#include "Graphics/Renderer.h"



namespace JG
{
	class IRootSignature;
	class IReadBackBuffer;
	class RayTracer;

	
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


		SharedPtr<RayTracer> mRayTracer;
	public:
		FowardRenderer();
		virtual ~FowardRenderer() = default;
	public:
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::Foward; }
		virtual void ReadyImpl(Graphics::RenderPassData* renderPassData, const RenderInfo& info) override;
		virtual void RenderImpl(const RenderInfo& info, SharedPtr<RenderResult> result) override;
		virtual void CompeleteImpl(const RenderInfo& info, SharedPtr<RenderResult> result) override;
		virtual int ArrangeObject(const ObjectInfo& info) override;
	private:
		void InitTextures(const JVector2& size, const Color& clearColor);
		void InitProcesses();
		void InitRayTracing();
		void InitGlobalRenderParams();

	};
}