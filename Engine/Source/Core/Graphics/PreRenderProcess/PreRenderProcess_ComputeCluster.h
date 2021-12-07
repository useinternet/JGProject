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
			JVector3 EyePosition;
			f32 FarZ;
			JVector2 Resolution;
			JVector2 TileSize;
			f32 NearZ;
		};
		CB CB;

		struct Cluster
		{
			JVector4 Min;
			JVector4 Max;
		};
		List<Cluster> Clusters;
	private:
		List<SharedPtr<IComputer>>       mComputers;
		List<SharedPtr<IReadBackBuffer>> mReadBackBuffers;
		SharedPtr<ScheduleHandle> mScheduleHandle;
	public:
		PreRenderProcess_ComputeCluster();
	public:
		virtual void Run(IGraphicsAPI* api, const RenderInfo& info) override;
		virtual bool IsCompelete() override;
	};
}