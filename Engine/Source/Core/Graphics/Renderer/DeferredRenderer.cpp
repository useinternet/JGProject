#include "pch.h"
#include "DeferredRenderer.h"
#include "Application.h"
#include "Graphics/JGGraphics.h"


namespace JG
{
	DeferredRenderer::DeferredRenderer()
	{

	}
	void DeferredRenderer::ReadyImpl(Graphics::RenderPassData* data, const RenderInfo& info)
	{
	}
	void DeferredRenderer::RenderImpl(const RenderInfo& info, SharedPtr<RenderResult> result)
	{
	}
	int DeferredRenderer::ArrangeObject(const ObjectInfo& info)
	{
		return 0;
	}
}