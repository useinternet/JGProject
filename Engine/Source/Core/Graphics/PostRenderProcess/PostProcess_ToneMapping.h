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
		// �׸� �ؽ���
		List<SharedPtr<ITexture>>  mToneMappingTextures;
		List<SharedPtr<ITexture>>  mOutLumaTextures;
		// Dispatch �� Computer
		List<SharedPtr<IComputer>> mToneMappingComputers;


		RP_Local_Bool mEnable;
		

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