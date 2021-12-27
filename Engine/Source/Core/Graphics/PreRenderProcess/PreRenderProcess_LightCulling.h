#pragma once
#include "Graphics/RenderProcess.h"






namespace JG
{
	//namespace 
	class IComputer;
	class IReadBackBuffer;
	class PreRenderProcess_ComputeCluster;
	class PreRenderProcess_LightCulling : public IRenderProcess
	{
		static const i32 NUM_RBB_COUNT = 2;
	public:
		static const i32 MAX_VISIBLE_LIGHTINDEX_COUNT = _256KB / sizeof(u32);


		const static constexpr char* SHADER_NAME = "LightCulling";
		const static constexpr char* SHADERPARAM_VIEWMATRIX				= "_ViewMatrix";
		const static constexpr char* SHADERPARAM_POINTLIGHTCOUNT		= "_PointLightCount";
		const static constexpr char* SHADERPARAM_NUM_X_SLICE = "_NUM_X_SLICE";
		const static constexpr char* SHADERPARAM_NUM_Y_SLICE = "_NUM_Y_SLICE";
		const static constexpr char* SHADERPARAM_NUM_Z_SLICE = "_NUM_Z_SLICE";
		const static constexpr char* SHADERPARAM_POINTLIGHTS			= "_PointLights";
		const static constexpr char* SHADERPARAM_CLUSTERS				= "_Clusters";
		const static constexpr char* SHADERPARAM_VISIBLE_LIGHTINDICES   = "_VisibleLightIndices";
		const static constexpr char* SHADERPARAM_LIGHTGRIDS				= "_LightGrids";

		struct CB
		{
			JMatrix		 ViewMatirx;
			i32			 PointLightCount = 0;
		};
		CB CB;

		List<Graphics::LightGrid> LightGrids;
		List<u32> VisibleLightIndices;
	private:
		JMatrix mPrevViewMatrix;
		SharedPtr<IComputer>       mComputer;
		SharedPtr<IReadBackBuffer> mVisibleLightIndicesRBB;
		SharedPtr<IReadBackBuffer> mLightGridRBB;

		bool mEnableDispatch = true;
		i32  mCompeleteRBBCount = 0;
		Renderer* mRenderer = nullptr;
		PreRenderProcess_ComputeCluster* mComputeClusterProcess  = nullptr;
	public:
		PreRenderProcess_LightCulling();
	public:
		virtual void Awake(Renderer* renderer) override;
		virtual void Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info) override;
		virtual void Run(Renderer* renderer,IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result) override;
		virtual bool IsCompelete() override;
		virtual Type GetType() const override;
	public:
		void SetClusters(const void* data, u64 dataCount, u64 dataSize);
	};
}