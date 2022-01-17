#pragma once
#include "Graphics/Renderer.h"



namespace JG
{
	class DeferredRenderer : public Renderer
	{
		enum class EGBuffer
		{
			Albedo,
			Normal,
			Material_A0, // Specular, Metalic, Roughness, A0
		};

		Dictionary<EGBuffer, List<SharedPtr<ITexture>>> mGBufferTexture;
		List<SharedPtr<ITexture>>					    mLightTexture;
		List<SharedPtr<ITexture>>						mShadowTexture;
	public:
		DeferredRenderer();
		virtual ~DeferredRenderer() = default;
	public:
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::Deferred; }
		virtual void ReadyImpl(Graphics::RenderPassData*  data, const RenderInfo& info) override;
		virtual void RenderImpl(const RenderInfo& info, SharedPtr<RenderResult> result) override;
		virtual void CompeleteImpl(const RenderInfo& info, SharedPtr<RenderResult> result) override {}
		virtual int ArrangeObject(const ObjectInfo & info) override;
	};
}