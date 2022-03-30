#pragma once
#include "Graphics/Renderer.h"



namespace JG
{
	class RayTracer;
	class IGraphicsShader;
	class ITexture;
	class DeferredRenderer : public Renderer
	{
		List<SharedPtr<ITexture>> mTargetTextures;
		List<SharedPtr<ITexture>> mTargetDepthTextures;
		JVector2 mResolution;
		Color    mClearColor;

		enum EGBuffer
		{
			Albedo,
			Normal,
			WorldPos,
			Specular,
			Emissive,
			Material_0, // Rough, Metallic, µîµî
			Depth
		};
		Dictionary<EGBuffer, List<SharedPtr<ITexture>>> mGBufferDic;
		Dictionary<EGBuffer, RP_Global_Tex> mGBufferTexDic;
		RP_Global_Tex mLightResultTex;
		RP_Global_Tex mFinalResultTex;
		List<SharedPtr<ITexture>>  mLightResult;
		SharedPtr<IGraphicsShader> mLightShader;
		SharedPtr<IGraphicsShader> mFinalShader;
	public:
		DeferredRenderer();
		virtual ~DeferredRenderer() = default;
	public:
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::Deferred; }
		virtual void ReadyImpl(Graphics::RenderPassData* renderPassData) override;
		virtual void RenderImpl(SharedPtr<RenderResult> result) override;
		virtual void CompeleteImpl(SharedPtr<RenderResult> result) override;
		virtual int ArrangeObject(const ObjectInfo& info) override;
	private:
		void InitTextures(const JVector2& size, const Color& clearColor);
		void InitProcesses();
		void InitGlobalRenderParams();
		void UpdateGBufferPass();
		void UpdateLightPass();
		void UpdateFinalPass();

		SharedPtr<ITexture> GetTargetTexture(EGBuffer buffer);
	};
}