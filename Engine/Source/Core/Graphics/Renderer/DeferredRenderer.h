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
		virtual int ArrangeObject(const ObjectInfo & info) override;
	private:
		void Draw(int objectType, const List<ObjectInfo>&objectList);
	private: // GBUffer
		void GBuffer_Init();
		void GBUffer_Ready(IGraphicsAPI* api, const RenderInfo& info);
		void GBuffer_Draw(int objectType, const List<ObjectInfo>& objectList);
	private: // Light
		void Light_Init();
		void Light_Ready();
		void Light_Draw();
	};
}