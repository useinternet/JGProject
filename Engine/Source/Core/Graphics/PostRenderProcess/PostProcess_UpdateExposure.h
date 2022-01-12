#pragma once
#include "Graphics/RenderProcess.h"

namespace JG
{
	class IMaterial;
	class PostProcess_UpdateExposure : public IRenderProcess
	{
		List<SharedPtr<IComputer>> mGenerateHistrogramComputers;
		List<SharedPtr<IComputer>> mAdaptExposureComputers;
		List<SharedPtr<IByteAddressBuffer>> mHistograms;
		


		List<IComputer> mAdaptLumaComputers;

		
	public:
		PostProcess_UpdateExposure();
	public:
		virtual void Awake(Renderer* renderer) override;
		virtual void Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info) override;
		virtual void Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result) override;
		virtual bool IsCompelete() override;
		virtual Type GetType() const override;
	private:
		bool InitComputers();
		bool InitTextures();
	};
}