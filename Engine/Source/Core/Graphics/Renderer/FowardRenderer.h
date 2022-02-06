#pragma once
#include "Graphics/Renderer.h"



namespace JG
{

	class IReadBackBuffer;
	class IMesh;
	class RayTracer;
	class IGraphicsShader;
	
	class FowardRenderer : public Renderer
	{
		List<SharedPtr<ITexture>> mTargetTextures;
		List<SharedPtr<ITexture>> mTargetDepthTextures;
	
		SharedPtr<RayTracer> mRayTracer;

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
		RP_Global_Bool mIsRayTracing;
		List<SharedPtr<ITexture>>  mLightResult;
		SharedPtr<IGraphicsShader> mLightShader;

		SharedPtr<IGraphicsShader> mFinalShader;
		List<SharedPtr<ITexture>>  mFinalResult;

	public:
		FowardRenderer();
		virtual ~FowardRenderer() = default;
	public:
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::Foward; }
		virtual void ReadyImpl(Graphics::RenderPassData* renderPassData) override;
		virtual void RenderImpl(SharedPtr<RenderResult> result) override;
		virtual void CompeleteImpl(SharedPtr<RenderResult> result) override;
		virtual int ArrangeObject(const ObjectInfo& info) override;
	private:
		void InitTextures(const JVector2& size, const Color& clearColor);
		void InitProcesses();
		void InitRayTracing();
		void InitGlobalRenderParams();

		void UpdateGBufferPass();
		void UpdateLightPass();
		void UpdateFinalPass();
		void UpdateRayTacing();
		
		SharedPtr<ITexture> GetTargetTexture(EGBuffer buffer);
	};
}