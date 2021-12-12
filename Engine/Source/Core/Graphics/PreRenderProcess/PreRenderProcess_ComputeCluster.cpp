#include "pch.h"
#include "PreRenderProcess_ComputeCluster.h"
#include "Graphics/JGGraphics.h"


#define COMPUTE_CLUSTER_SHADER_NAME "ComputeCluster"
#define NUM_CLUSTER 1024

#define SHADERPARAM_INVPROJMATRIX "__InvProjMatrix__"
#define SHADERPARAM_EYEPOSITION "__EyePosition__"
#define SHADERPARAM_RESOLUTION "__Resolution__"
#define SHADERPARAM_TILESIZE "__TileSize__"
#define SHADERPARAM_NEARZ "__NearZ__"
#define SHADERPARAM_FARZ "__FarZ__"
#define SHADERPARAM_CLUSTERS "Clusters"

namespace JG
{
	PreRenderProcess_ComputeCluster::PreRenderProcess_ComputeCluster()
	{
		u64 bufferCnt = JGGraphics::GetInstance().GetBufferCount();


		auto shader = ShaderLibrary::GetInstance().FindComputeShader(COMPUTE_CLUSTER_SHADER_NAME);
		if (shader != nullptr)
		{
			for (u64 i = 0; i < bufferCnt; ++i)
			{
				//auto computer = IComputer::Create("ClusterComputer", shader);
				//auto readBackBuffer = IReadBackBuffer::Create("ClusterComputer_ReadBackBuffer");

				//mComputers.push_back(computer);
				//mReadBackBuffers.push_back(readBackBuffer);
			}
		}
	}
	void PreRenderProcess_ComputeCluster::Run(IGraphicsAPI* api, const RenderInfo& info)
	{
		if (mComputers.empty() || mReadBackBuffers.empty())
		{
			return;
		}
		
		mScheduleHandle = Scheduler::GetInstance().ScheduleAsync([&]()
		{
			u64  bufferCnt = JGGraphics::GetInstance().GetBufferCount();
			auto commandID = JGGraphics::GetInstance().RequestCommandID();
			i32 bufferIndex = info.CurrentBufferIndex;


			// Dispatch
			{

				// 0
				auto computer = mComputers[bufferIndex];
				computer->SetFloat4x4(SHADERPARAM_INVPROJMATRIX, CB.InvProjMatrix);
				computer->SetFloat3(SHADERPARAM_EYEPOSITION, CB.EyePosition);
				computer->SetFloat2(SHADERPARAM_RESOLUTION, CB.Resolution);
				computer->SetFloat2(SHADERPARAM_TILESIZE, CB.TileSize);
				computer->SetFloat(SHADERPARAM_NEARZ, CB.NearZ);
				computer->SetFloat(SHADERPARAM_FARZ, CB.FarZ);
				computer->Dispatch(commandID, 1, 1, 1);
			}

			// Read
			{
				auto computerIndex = bufferIndex - 1;
				if (computerIndex < 0) computerIndex = bufferCnt - 1;
				auto rwBuffer = mComputers[computerIndex]->GetRWBuffer(SHADERPARAM_CLUSTERS);
				auto readBackBuffer = mReadBackBuffers[bufferIndex];

				if (rwBuffer != nullptr && rwBuffer->IsValid())
				{
					readBackBuffer->Read(rwBuffer);
				}
			}

			// »Æ¿Œ
			{
				auto readBackBufferIndex = bufferIndex - 1;
				if (readBackBufferIndex < 0) readBackBufferIndex = bufferCnt - 1;


				auto readBackBuffer = mReadBackBuffers[readBackBufferIndex];
				if (readBackBuffer->IsValid() && readBackBuffer->GetState() == EReadBackBufferState::ReadCompelete)
				{
					Clusters.resize(NUM_CLUSTER);
					readBackBuffer->GetData(Clusters.data(), sizeof(Cluster) * NUM_CLUSTER);
				}
			}




		});

		
	}

	bool PreRenderProcess_ComputeCluster::IsCompelete()
	{
		return mScheduleHandle->GetState() == EScheduleState::Compelete;
	}
}

