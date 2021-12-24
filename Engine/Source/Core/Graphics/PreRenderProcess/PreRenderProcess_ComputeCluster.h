#pragma once
#include "Graphics/RenderProcess.h"






namespace JG
{
	class IComputer;
	class IReadBackBuffer;

	class PreRenderProcess_LightCulling;

	class PreRenderProcess_ComputeCluster : public IRenderProcess
	{
	
	public:
		const static i32 NUM_X_SLICE = 16;
		const static i32 NUM_Y_SLICE = 9;
		const static i32 NUM_Z_SLICE = 24;
		const static i32 NUM_CLUSTER = NUM_X_SLICE * NUM_Y_SLICE * NUM_Z_SLICE;

		const static constexpr char* SHADER_NAME = "ComputeCluster";
		const static constexpr char* SHADERPARAM_INVPROJMATRIX = "__InvProjMatrix__";
		const static constexpr char* SHADERPARAM_RESOLUTION	   = "__Resolution__";
		const static constexpr char* SHADERPARAM_TILESIZE	   = "__TileSize__";
		const static constexpr char* SHADERPARAM_NEARZ		   = "__NearZ__";
		const static constexpr char* SHADERPARAM_FARZ		   = "__FarZ__";
		const static constexpr char* SHADERPARAM_CLUSTERS	   = "_Clusters";
		const static constexpr char* SHADERPARAM_NUM_X_SLICE = "_NUM_X_SLICE";
		const static constexpr char* SHADERPARAM_NUM_Y_SLICE = "_NUM_Y_SLICE";
		const static constexpr char* SHADERPARAM_NUM_Z_SLICE = "_NUM_Z_SLICE";
		struct CB
		{
			JMatrix  InvProjMatrix;
			JVector2 Resolution;
			JVector2 TileSize;
			f32 FarZ = 0.0f;
			f32 NearZ = 0.0f;
		};
		CB CB;

		struct Cluster
		{
			JVector4 Min;
			JVector4 Max;
		};

		List<Cluster>     Clusters;
	private:
		JMatrix mPrevProjMatrix;
		SharedPtr<IComputer>       mComputer;
		SharedPtr<IReadBackBuffer> mClusterRBB;

		bool mIsDirty        = true;
		bool mEnableDispatch = true;

		PreRenderProcess_LightCulling* mLightCullingProcess = nullptr;
	public:
		PreRenderProcess_ComputeCluster();
	public:
		virtual void Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info) override;
		virtual void Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info) override;
		virtual bool IsCompelete() override;
		virtual Type GetType() const override;
	private:
		bool CheckDirty(const RenderInfo& info);
	};
}