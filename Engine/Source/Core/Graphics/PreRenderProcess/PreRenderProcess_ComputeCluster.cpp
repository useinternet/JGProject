#include "pch.h"
#include "PreRenderProcess_ComputeCluster.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/Renderer.h"

#include "PreRenderProcess_LightCulling.h"

namespace JG
{
	PreRenderProcess_ComputeCluster::PreRenderProcess_ComputeCluster()
	{
		auto shader   = ShaderLibrary::GetInstance().FindComputeShader(SHADER_NAME);
		if (shader != nullptr)
		{
			mComputer =  IComputer::Create("ClusterComputer", shader);
			mClusterSB = IStructuredBuffer::Create("ClusterSB", sizeof(Cluster), NUM_CLUSTER);
			mComputer->SetStructuredBuffer(SHADERPARAM_CLUSTERS, mClusterSB);
			mComputer->SetUint(SHADERPARAM_NUM_X_SLICE, NUM_X_SLICE);
			mComputer->SetUint(SHADERPARAM_NUM_Y_SLICE, NUM_Y_SLICE);
			mComputer->SetUint(SHADERPARAM_NUM_Z_SLICE, NUM_Z_SLICE);
		}
	}
	void PreRenderProcess_ComputeCluster::Awake(Renderer* renderer)
	{
		

	}
	void PreRenderProcess_ComputeCluster::Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info)
	{
		rednerPassData->NumClusterSlice.x = NUM_X_SLICE;
		rednerPassData->NumClusterSlice.y = NUM_Y_SLICE;
		rednerPassData->NumClusterSlice.z = NUM_Z_SLICE;
		rednerPassData->ClusterSize.x = (u32)std::ceilf(info.Resolution.x / (float)NUM_X_SLICE);
		rednerPassData->ClusterSize.y = (u32)std::ceilf(info.Resolution.y / (float)NUM_Y_SLICE);
		rednerPassData->ClusterScale  = (f32)NUM_Z_SLICE / std::log2f(info.FarZ / info.NearZ);
		rednerPassData->ClusterBias   = -((f32)NUM_Z_SLICE * std::log2f(info.NearZ) / std::log2f(info.FarZ / info.NearZ));



		if (CheckDirty(info) == false)
		{
			return;
		}
		mIsDirty = true;
	

		CB.InvProjMatrix = JMatrix::Transpose(JMatrix::Inverse(info.ProjMatrix));
		CB.Resolution    = info.Resolution;
		CB.FarZ			 = info.FarZ;
		CB.NearZ		 = info.NearZ;
		CB.TileSize = JVector2(
			info.Resolution.x / (f32)PreRenderProcess_ComputeCluster::NUM_X_SLICE,
			info.Resolution.y / (f32)PreRenderProcess_ComputeCluster::NUM_Y_SLICE);


		mComputer->SetFloat4x4(SHADERPARAM_INVPROJMATRIX, CB.InvProjMatrix);
		mComputer->SetFloat2(SHADERPARAM_RESOLUTION, CB.Resolution);
		mComputer->SetFloat2(SHADERPARAM_TILESIZE, CB.TileSize);
		mComputer->SetFloat(SHADERPARAM_NEARZ, CB.NearZ);
		mComputer->SetFloat(SHADERPARAM_FARZ, CB.FarZ);
	}
	void PreRenderProcess_ComputeCluster::Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{
		if (mLightCullingProcess == nullptr)
		{
			mLightCullingProcess = renderer->FindProcess<PreRenderProcess_LightCulling>();
			if (mLightCullingProcess)
			{
				mLightCullingProcess->SetClusters(mClusterSB);
			}
		}


		if (mComputer == nullptr || mIsDirty == false)
		{
			return;
		}
		mComputer->Dispatch(NUM_X_SLICE, NUM_Y_SLICE, NUM_Z_SLICE, false);
		Scheduler::GetInstance().ScheduleOnceByFrame(JGGraphics::GetInstance().GetBufferCount() + 1, 0,
			[&]()->EScheduleResult
		{
			mIsDirty = false;
			return EScheduleResult::Break;
		});
	}

	bool PreRenderProcess_ComputeCluster::IsCompelete()
	{
		return true;
	}

	Type PreRenderProcess_ComputeCluster::GetType() const
	{
		return JGTYPE(PreRenderProcess_ComputeCluster);
	}

	bool PreRenderProcess_ComputeCluster::CheckDirty(const RenderInfo& info)
	{
		for (i32 i = 0; i < 4; ++i)
		{
			for (i32 j = 0; j < 4; ++j)
			{
				if (mPrevProjMatrix.Get_C(i, j) != info.ProjMatrix.Get_C(i, j))
				{
					mPrevProjMatrix = info.ProjMatrix;
					return true;
				}
			}
		}

		if (CB.Resolution != info.Resolution)
		{
			return true;
		}

		if (CB.FarZ != info.FarZ)
		{
			return true;
		}
		if (CB.NearZ != info.NearZ)
		{
			return true;
		}


		return false;
	}
}

