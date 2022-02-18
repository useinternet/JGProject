#include "pch.h"
#include "PreRenderProcess_ComputeCluster.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/Renderer.h"
#include "Graphics/RootSignature.h"
#include "PreRenderProcess_LightCulling.h"

namespace JG
{
	void PreRenderProcess_ComputeCluster::Awake(const AwakeData& data)
	{
		mShader = ShaderLibrary::GetInstance().FindComputeShader(SHADER_NAME);
		SharedPtr<IRootSignatureCreater> rootSigCreater = IRootSignatureCreater::Create();

		rootSigCreater->AddCBV(0, 0, 0);
		rootSigCreater->AddUAV(1, 0, 0);

		mRootSignature = rootSigCreater->Generate();
		mClusterSB = IStructuredBuffer::Create("ClusterSB", sizeof(Cluster), NUM_CLUSTER, EBufferLoadMethod::GPULoad);
	}
	void PreRenderProcess_ComputeCluster::Ready(const ReadyData& data)
	{
		data.pRenderPassData->NumClusterSlice.x = NUM_X_SLICE;
		data.pRenderPassData->NumClusterSlice.y = NUM_Y_SLICE;
		data.pRenderPassData->NumClusterSlice.z = NUM_Z_SLICE;
		data.pRenderPassData->ClusterSize.x = (u32)std::ceilf(data.Info.Resolution.x / (float)NUM_X_SLICE);
		data.pRenderPassData->ClusterSize.y = (u32)std::ceilf(data.Info.Resolution.y / (float)NUM_Y_SLICE);
		data.pRenderPassData->ClusterScale  = (f32)NUM_Z_SLICE / std::log2f(data.Info.FarZ / data.Info.NearZ);
		data.pRenderPassData->ClusterBias   = -((f32)NUM_Z_SLICE * std::log2f(data.Info.NearZ) / std::log2f(data.Info.FarZ / data.Info.NearZ));



		if (CheckDirty(data.Info) == false)
		{
			return;
		}
		mIsDirty = true;
	

		CB.InvProjMatrix = JMatrix::Transpose(JMatrix::Inverse(data.Info.ProjMatrix));
		CB.Resolution    = data.Info.Resolution;
		CB.FarZ			 = data.Info.FarZ;
		CB.NearZ		 = data.Info.NearZ;
		CB.TileSize = JVector2(
			data.Info.Resolution.x / (f32)PreRenderProcess_ComputeCluster::NUM_X_SLICE,
			data.Info.Resolution.y / (f32)PreRenderProcess_ComputeCluster::NUM_Y_SLICE);
		CB.NumXSlice = NUM_X_SLICE;
		CB.NumYSlice = NUM_Y_SLICE;
		CB.NumZSlice = NUM_Z_SLICE;
	}
	void PreRenderProcess_ComputeCluster::Run(const RunData& data)
	{
		if (mLightCullingProcess == nullptr)
		{
			mLightCullingProcess = data.pRenderer->FindProcess<PreRenderProcess_LightCulling>();
			if (mLightCullingProcess)
			{
				mLightCullingProcess->SetClusters(mClusterSB);
			}
		}


		if (mIsDirty == false)
		{
			return;
		}
		mIsDirty = false;


		SharedPtr<IComputeContext> context = data.ComputeContext;
		context->BindShader(mShader);
		context->BindRootSignature(mRootSignature);
		context->BindConstantBuffer(0, CB);
		context->BindSturcturedBuffer(1, mClusterSB);
		context->Dispatch(NUM_X_SLICE, NUM_Y_SLICE, NUM_Z_SLICE);

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

