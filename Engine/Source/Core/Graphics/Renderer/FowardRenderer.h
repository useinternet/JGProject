#pragma once
#include "Graphics/Renderer.h"



namespace JG
{

	class IReadBackBuffer;
	class IMesh;
	class RayTracer;

	
	class FowardRenderer : public Renderer
	{
		List<SharedPtr<ITexture>> mTargetTextures;
		List<SharedPtr<ITexture>> mTargetDepthTextures;
	
		SharedPtr<RayTracer> mRayTracer;

		JVector2 mPrevResolution;
		Color    mPrevClearColor;




		List<SharedPtr<IStructuredBuffer>> mExposureSB;
		RP_Global_Float mExposure;
		RP_Global_Float mInitialMinLog;
		RP_Global_Float mInitialMaxLog;



	public:
		FowardRenderer();
		virtual ~FowardRenderer() = default;
	public:
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::Foward; }
		virtual void ReadyImpl(Graphics::RenderPassData* renderPassData) override;
		virtual void RenderImpl(SharedPtr<RenderResult> result) override;
		virtual void CompeleteImpl(SharedPtr<RenderResult> result) override;
		virtual int ArrangeObject(const ObjectInfo& info) override;
	private:
		void InitTextures(const JVector2& size, const Color& clearColor);
		void InitProcesses();
		void InitRayTracing();
		void InitGlobalRenderParams();

		void UpdateBottomLevelAS(SharedPtr<IMesh> mesh, const JMatrix& worldMatrix);
		void UpdateRayTacing();

	};
}