#pragma once
#include "Graphics/RenderProcess.h"






namespace JG
{
	class IComputer;
	class IReadBackBuffer;


	class PreRenderProcess_ComputeCluster : public IRenderProcess
	{
		// 
	public:
		const static i32 NUM_X_SLICE = 8;
		const static i32 NUM_Y_SLICE = 8;
		const static i32 NUM_Z_SLICE = 16;
		struct CB
		{
			JMatrix InvProjMatrix;
			JMatrix ViewMatrix;
			JVector3 EyePosition;
			f32 FarZ = 0.0f;
			JVector2 Resolution;
			JVector2 TileSize;
			f32 NearZ = 0.0f;
			i32 PointLightCount = 0;
		};
		CB CB;

		struct Cluster
		{
			JVector4 Min;
			JVector4 Max;
		};
		struct ClusterInfo
		{
			i32 PL_Count  = 0;
			i32 PL_Offset = 0;
			i32 Enable = 0;
		};
		List<ClusterInfo> mClusterInfos;
		List<u32>		  mLightIndices;
	private:
		List<SharedPtr<IComputer>>       mComputers;
		List<SharedPtr<IReadBackBuffer>> mLightIndicesRBB;
		List<SharedPtr<IReadBackBuffer>> mClusterInfosRBB;
		SharedPtr<ScheduleHandle> mScheduleHandle;
	public:
		PreRenderProcess_ComputeCluster();
	public:
		virtual void Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info) override;
		virtual bool IsCompelete() override;
	private:
		void ReadData(i32 bufferIndex, const String& paramName, const List<SharedPtr<IReadBackBuffer>>& rbList,
			const std::function<void(SharedPtr<IReadBackBuffer>)>& action);
	};
}