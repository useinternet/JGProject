#pragma once
#include "Graphics/RenderProcess.h"

namespace JG
{
	class IMaterial;
	class PostProcess_Bloom : public IRenderProcess
	{
		struct Data
		{
			SharedPtr<IMaterial>	  Material;
			List<SharedPtr<ITexture>> Textures;
		};
		

		
		//Data mBrightnessData;



		u32 mDownSamplingCount = 1;
		u32 mBlurCount = 5;
		SharedPtr<IMaterial>			mBlurHMaterial;
		List<List<SharedPtr<ITexture>>> mBlurHTextures;


		SharedPtr<IMaterial>			mBlurVMaterial;
		List<List<SharedPtr<ITexture>>> mBlurVTextures;


		SharedPtr<IMaterial> mBrightnessMaterial;
		List<List<SharedPtr<ITexture>>> mBrightnessTextures;


		//Data mBlurHData;
		//Data mBlurVData;
		Data mBloomData;


		JVector2 mPrevResolution;
		JVector2 mDownSamplingResolution;


		// DowanSampling 하면서 Brightness
		// UpSampling 하면서 Blur 멕이자
	public:
		PostProcess_Bloom();
	public:
		virtual void Awake(Renderer* renderer) override;
		virtual void Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info) override;
		virtual void Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result) override;
		virtual bool IsCompelete() override;
		virtual Type GetType() const override;
	private:
		SharedPtr<ITexture> Run_Brightness(u64 commandID, const RenderInfo& info, i32 index, const JVector2& resolution, SharedPtr<ITexture> sceneTexture);
		SharedPtr<ITexture> Run_BlurH(u64 commandID, const RenderInfo& info, i32 index, const JVector2& resolution, SharedPtr<ITexture> sceneTexture);
		SharedPtr<ITexture> Run_BlurV(u64 commandID, const RenderInfo& info, i32 index, const JVector2& resolution, SharedPtr<ITexture> sceneTexture);
		SharedPtr<ITexture> Run_Bloom(u64 commandID, const RenderInfo& info, i32 index, SharedPtr<ITexture> sceneTexture, SharedPtr<ITexture> brightnessTexture);
	private:
		void InitMaterial();
		void InitTexture(const JVector2& resolution);
	};
}