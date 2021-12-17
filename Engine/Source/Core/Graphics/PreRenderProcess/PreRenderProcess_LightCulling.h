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
	public:
		static const i32 MAX_VISIBLE_LIGHTINDEX_COUNT = _128KB / sizeof(u32);


		const static constexpr char* SHADER_NAME = "LightCulling";
		const static constexpr char* SHADERPARAM_VIEWMATRIX				= "_ViewMatrix";
		const static constexpr char* SHADERPARAM_POINTLIGHTCOUNT		= "_PointLightCount";
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
		bool mIsDataReading  = false;
		Renderer* mRenderer = nullptr;
		PreRenderProcess_ComputeCluster* mComputeClusterProcess  = nullptr;
	public:
		PreRenderProcess_LightCulling();
	public:
		virtual void Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info) override;
		virtual void Run(Renderer* renderer,IGraphicsAPI* api, const RenderInfo& info) override;
		virtual bool IsCompelete() override;
		virtual Type GetType() const override;
	public:
		void SetClusters(const void* data, u64 dataCount, u64 dataSize);
	};
}