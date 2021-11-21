#include "pch.h"
#include "DeferredRenderer.h"
#include "Application.h"
#include "Graphics/JGGraphics.h"


namespace JG
{
	DeferredRenderer::DeferredRenderer()
	{

	}
	int DeferredRenderer::ArrangeObject(const ObjectInfo& info)
	{
		return 0;
	}
	void DeferredRenderer::Draw(int objectType, const List<ObjectInfo>& objectList)
	{
	}
	void DeferredRenderer::GBuffer_Init()
	{
		auto bufferCnt = JGGraphics::GetInstance().GetBufferCount();
		mGBufferTexture[EGBuffer::Albedo].resize(bufferCnt, nullptr);
		mGBufferTexture[EGBuffer::Normal].resize(bufferCnt, nullptr);
		mGBufferTexture[EGBuffer::Material_A0].resize(bufferCnt, nullptr);
	}
	void DeferredRenderer::GBUffer_Ready(IGraphicsAPI* api, const RenderInfo& info)
	{
		auto targetTextureInfo = info.TargetTexture->GetTextureInfo();









	}
	void DeferredRenderer::GBuffer_Draw(int objectType, const List<ObjectInfo>& objectList)
	{
	}
	void DeferredRenderer::Light_Init()
	{
	}
	void DeferredRenderer::Light_Ready()
	{
	}
	void DeferredRenderer::Light_Draw()
	{
	}
}