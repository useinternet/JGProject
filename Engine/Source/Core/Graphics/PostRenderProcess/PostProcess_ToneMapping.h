#pragma once
#include "Graphics/RenderProcess.h"

namespace JG
{
	class IMaterial;
	class PostProcess_ToneMapping : public IRenderProcess
	{
	public:
		const static constexpr char* SCRIPT_NAME = "Scene/ToneMapping";
		const static constexpr char* MATERIAL_PARAM_SCENETEXTURE = "SceneTexture";
		const static constexpr char* MATERIAL_PARAM_EXPOSURE     = "Exposure";

	private:
		// 그릴 텍스쳐
		List<SharedPtr<ITexture>>  mTargetTextures;
		// Dispatch 할 Computer
		List<SharedPtr<IComputer>> mComputers;


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
		void InitComputers();
		void InitTextures(const JVector2& size);
	};
}