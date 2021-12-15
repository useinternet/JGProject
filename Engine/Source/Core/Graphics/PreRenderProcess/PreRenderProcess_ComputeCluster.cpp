#include "pch.h"
#include "PreRenderProcess_ComputeCluster.h"
#include "Graphics/JGGraphics.h"


#define COMPUTE_CLUSTER_SHADER_NAME "ComputeCluster"
#define NUM_CLUSTER 1024

#define SHADERPARAM_INVPROJMATRIX "__InvProjMatrix__"
#define SHADERPARAM_VIEWMATRIX "__ViewMatrix__"
#define SHADERPARAM_EYEPOSITION "__EyePosition__"
#define SHADERPARAM_RESOLUTION "__Resolution__"
#define SHADERPARAM_TILESIZE "__TileSize__"
#define SHADERPARAM_NEARZ "__NearZ__"
#define SHADERPARAM_FARZ "__FarZ__"
#define SHADERPARAM_POINTLIGHTCOUNT "__PointLightCount__"


#define SHADERPARAM_POINTLIGHTS   "_PointLights"
#define SHADERPARAM_LIGHTINDICIES "_ClusterLightIndices"
#define SHADERPARAM_CLUSTERINFOS  "_ClusterInfos"

namespace JG
{
	PreRenderProcess_ComputeCluster::PreRenderProcess_ComputeCluster()
	{
		u64 bufferCnt = JGGraphics::GetInstance().GetBufferCount();
		auto shader   = ShaderLibrary::GetInstance().FindComputeShader(COMPUTE_CLUSTER_SHADER_NAME);
		if (shader != nullptr)
		{
			for (u64 i = 0; i < bufferCnt; ++i)
			{
				auto computer = IComputer::Create("ClusterComputer", shader);
				auto ll_rbb = IReadBackBuffer::Create("ClusterComputer_LightIndicesRBB");
				auto ci_rbb = IReadBackBuffer::Create("ClusterComputer_ClusterInfosRBB");

				mComputers.push_back(computer);
				mLightIndicesRBB.push_back(ll_rbb);
				mClusterInfosRBB.push_back(ci_rbb);
			}
		}

		//  sizeX =  (unsigned int)std::ceilf(DisplayManager::SCREEN_WIDTH / (float)gridSizeX);
		/*
		  screen2View.tileSizes[0] = gridSizeX;
        screen2View.tileSizes[1] = gridSizeY;
        screen2View.tileSizes[2] = gridSizeZ;
        screen2View.tileSizes[3] = sizeX;
		screen2View.sliceScalingFactor = (float)gridSizeZ / std::log2f(zFar / zNear) ;
		screen2View.sliceBiasFactor    = -((float)gridSizeZ * std::log2f(zNear) / std::log2f(zFar / zNear)) ;
		*/
	}
	void PreRenderProcess_ComputeCluster::Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info)
	{
		if (mComputers.empty() || mLightIndicesRBB.empty() || mClusterInfosRBB.empty())
		{
			return;
		}
		
		mScheduleHandle = Scheduler::GetInstance().ScheduleAsync([&]()
		{
			u64  bufferCnt = JGGraphics::GetInstance().GetBufferCount();
			auto commandID = JGGraphics::GetInstance().RequestCommandID();
			i32 bufferIndex = info.CurrentBufferIndex;

			auto lightInfo = renderer->GetLightInfo(Graphics::ELightType::PointLight);
			// Dispatch
			{

				// 0
				auto computer = mComputers[bufferIndex];
				computer->SetFloat4x4(SHADERPARAM_INVPROJMATRIX, CB.InvProjMatrix);
				computer->SetFloat4x4(SHADERPARAM_VIEWMATRIX, CB.ViewMatrix);
				computer->SetFloat3(SHADERPARAM_EYEPOSITION, CB.EyePosition);
				computer->SetFloat2(SHADERPARAM_RESOLUTION, CB.Resolution);
				computer->SetFloat2(SHADERPARAM_TILESIZE, CB.TileSize);
				computer->SetFloat(SHADERPARAM_NEARZ, CB.NearZ);
				computer->SetFloat(SHADERPARAM_FARZ, CB.FarZ);
				computer->SetInt(SHADERPARAM_POINTLIGHTCOUNT, CB.PointLightCount);
				computer->SetStructDataArray(SHADERPARAM_POINTLIGHTS, lightInfo.ByteData.data(), lightInfo.Count, lightInfo.Size);
				computer->Dispatch(commandID, 1, 1, 1);
			}

			ReadData(bufferIndex, SHADERPARAM_LIGHTINDICIES, mLightIndicesRBB,
				[&](SharedPtr<IReadBackBuffer> rbBuffer)
			{
				mLightIndices.resize(10240);
				rbBuffer->GetData(mLightIndices.data(), sizeof(u32) * 10240);

			});

			ReadData(bufferIndex, SHADERPARAM_CLUSTERINFOS, mClusterInfosRBB,
				[&](SharedPtr<IReadBackBuffer> rbBuffer)
			{
				mClusterInfos.resize(NUM_CLUSTER);
				rbBuffer->GetData(mClusterInfos.data(), sizeof(ClusterInfo) * NUM_CLUSTER);
			});

		});

		
	}

	bool PreRenderProcess_ComputeCluster::IsCompelete()
	{
		if (mScheduleHandle == nullptr)
		{
			return true;
		}
		return mScheduleHandle->GetState() == EScheduleState::Compelete;
	}

	void PreRenderProcess_ComputeCluster::ReadData(i32 bufferIndex, const String& paramName, const List<SharedPtr<IReadBackBuffer>>& rbList, const std::function<void(SharedPtr<IReadBackBuffer>)>& action)
	{
		u64  bufferCnt = JGGraphics::GetInstance().GetBufferCount();
		// Read
		{
			auto computerIndex = bufferIndex - 1;
			if (computerIndex < 0) computerIndex = bufferCnt - 1;
			auto rwBuffer = mComputers[computerIndex]->GetRWBuffer(paramName);
			auto readBackBuffer = rbList[bufferIndex];

			if (rwBuffer != nullptr && rwBuffer->IsValid())
			{
				readBackBuffer->Read(rwBuffer);
			}
		}

		// »Æ¿Œ
		{
			auto readBackBufferIndex = bufferIndex - 1;
			if (readBackBufferIndex < 0) readBackBufferIndex = bufferCnt - 1;


			auto readBackBuffer = rbList[readBackBufferIndex];
			if (readBackBuffer->IsValid() && readBackBuffer->GetState() == EReadBackBufferState::ReadCompelete)
			{
				if (action != nullptr)
				{
					action(readBackBuffer);
				}
			}
		}
	}
}

