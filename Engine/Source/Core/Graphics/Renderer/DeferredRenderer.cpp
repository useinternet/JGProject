#include "pch.h"
#include "DeferredRenderer.h"
#include "Application.h"
#include "Graphics/JGGraphics.h"


namespace JG
{
	DeferredRenderer::DeferredRenderer()
	{

	}
	void DeferredRenderer::ReadyImpl(IGraphicsAPI* api, const RenderInfo& info)
	{
	}
	void DeferredRenderer::RenderImpl(IGraphicsAPI* api, const RenderInfo& info)
	{
	}
	int DeferredRenderer::ArrangeObject(const ObjectInfo& info)
	{
		return 0;
	}
}