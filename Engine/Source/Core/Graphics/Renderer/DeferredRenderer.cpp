#include "pch.h"
#include "DeferredRenderer.h"
#include "Application.h"
#include "Graphics/JGGraphics.h"


namespace JG
{
	DeferredRenderer::DeferredRenderer()
	{

	}
	void DeferredRenderer::ReadyImpl(IGraphicsAPI* api, Graphics::RenderPassData* data, const RenderInfo& info)
	{
	}
	void DeferredRenderer::RenderImpl(IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{
	}
	int DeferredRenderer::ArrangeObject(const ObjectInfo& info)
	{
		return 0;
	}
}