#pragma once
#include "Graphics/Renderer.h"



namespace JG
{
	class RayTracer;
	class RayTracingRenderer : public Renderer
	{
		SharedPtr<ITexture> mTargetTexture;
		SharedPtr<RayTracer> mRayTracer;

		JVector2 mResolution;
		Color    mClearColor;
	public:
		RayTracingRenderer();
		virtual ~RayTracingRenderer() = default;
	public:
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::RayTracing; }
		virtual void ReadyImpl(Graphics::RenderPassData* renderPassData) override;
		virtual void RenderImpl(SharedPtr<RenderResult> result) override;
		virtual void CompeleteImpl(SharedPtr<RenderResult> result) override;
		virtual int ArrangeObject(const ObjectInfo& info) override;
	private:
		void InitTextures(const JVector2& size, const Color& clearColor);
		void InitProcesses();
		void Init();
		void InitGlobalRenderParams();
	};
}