#pragma once
#include "Graphics/RenderProcess.h"

namespace JG
{
	class IMaterial;
	class PostProcess_ToneMapping : public IRenderProcess
	{
	private:
		// 그릴 텍스쳐
		List<SharedPtr<ITexture>>  mToneMappingTextures;
		List<SharedPtr<ITexture>>  mOutLumaTextures;
		// Dispatch 할 Computer
		List<SharedPtr<IComputer>> mToneMappingComputers;

		JVector2 mPrevResolution;
	public:
		PostProcess_ToneMapping();
	public:
		virtual void Awake(Renderer* renderer) override;
		virtual void Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info) override;
		virtual void Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result) override;
		virtual bool IsCompelete() override;
		virtual Type GetType() const override;
	private:
		bool InitComputers();
		bool InitTextures(const JVector2& size);
	};
}