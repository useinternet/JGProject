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
		virtual void ReadyImpl() override;
		virtual void RenderImpl(IGraphicsAPI* api, const RenderInfo& info) override;
		virtual int ArrangeObject(const ObjectInfo & info) override;
	};
}