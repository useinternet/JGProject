#pragma once
#include "Graphics/Renderer.h"



namespace JG
{
	class FowardRenderer : public Renderer
	{
	public:
		FowardRenderer();
		virtual ~FowardRenderer() = default;
	public:
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::Foward; }
		virtual void ReadyImpl(IGraphicsAPI* api, Graphics::RenderPassData* renderPassData, const RenderInfo& info) override;
		virtual void RenderImpl(IGraphicsAPI* api, const RenderInfo& info) override;
		virtual void CompeleteImpl(IGraphicsAPI* api, const RenderInfo& info) override;
		virtual int ArrangeObject(const ObjectInfo& info) override;
	};
}