#include "pch.h"
#include "PreRenderProcess_LightCulling.h"
#include "PreRenderProcess_ComputeCluster.h"
namespace JG
{
	PreRenderProcess_LightCulling::PreRenderProcess_LightCulling()
	{
		auto shader = ShaderLibrary::GetInstance().FindComputeShader(SHADER_NAME);
		if (shader != nullptr)
		{
			mComputer = IComputer::Create("LightCullingComputer", shader);
			mLightGridRBB = IReadBackBuffer::Create("LightGridRBB");
			mVisibleLightIndicesRBB = IReadBackBuffer::Create("VisibleLightIndicesRBB ");
		}

		LightGrids.resize(PreRenderProcess_ComputeCluster::NUM_CLUSTER);
		VisibleLightIndices.resize(MAX_VISIBLE_LIGHTINDEX_COUNT, 0);
	}
	void PreRenderProcess_LightCulling::Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* renderPassData, const RenderInfo& info)
	{
		if (mComputeClusterProcess == nullptr)
		{
			mComputeClusterProcess = renderer->FindProcess<PreRenderProcess_ComputeCluster>();
		}
		if (mRenderer == nullptr)
		{
			mRenderer = renderer;
		}
		auto commandID = JGGraphics::GetInstance().RequestCommandID();
		api->SetLightGrids(commandID, LightGrids);
		api->SetVisibleLightIndicies(commandID, VisibleLightIndices);


		CB.ViewMatirx = JMatrix::Transpose(info.ViewMatrix);
		CB.PointLightCount = renderer->GetLightInfo(Graphics::ELightType::PointLight).Count;

	}
	void PreRenderProcess_LightCulling::Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info)
	{
		if (mComputer == nullptr || mVisibleLightIndicesRBB == nullptr || mLightGridRBB == nullptr)
		{
			return;
		}




		if (mEnableDispatch == true)
		{
			auto commandID = JGGraphics::GetInstance().RequestCommandID();
			auto pointLightsInfo = mRenderer->GetLightInfo(Graphics::ELightType::PointLight);
			mComputer->SetFloat4x4(SHADERPARAM_VIEWMATRIX, CB.ViewMatirx);
			mComputer->SetInt(SHADERPARAM_POINTLIGHTCOUNT, CB.PointLightCount);
			mComputer->SetStructDataArray(SHADERPARAM_POINTLIGHTS, pointLightsInfo.ByteData.data(), pointLightsInfo.Count, pointLightsInfo.Size);
			mComputer->SetUint(SHADERPARAM_NUM_X_SLICE, PreRenderProcess_ComputeCluster::NUM_X_SLICE);
			mComputer->SetUint(SHADERPARAM_NUM_Y_SLICE, PreRenderProcess_ComputeCluster::NUM_Y_SLICE);
			mComputer->SetUint(SHADERPARAM_NUM_Z_SLICE, PreRenderProcess_ComputeCluster::NUM_Z_SLICE);
			mComputer->Dispatch(commandID, PreRenderProcess_ComputeCluster::NUM_X_SLICE, PreRenderProcess_ComputeCluster::NUM_Y_SLICE, PreRenderProcess_ComputeCluster::NUM_Z_SLICE);
			mEnableDispatch = false;
		}
		else
		{
			if (mComputer->GetState() == EComputerState::Compelete)
			{
				auto lightGridRWB		    = mComputer->GetRWBuffer(SHADERPARAM_LIGHTGRIDS);
				auto visibleLightIndicesRWB = mComputer->GetRWBuffer(SHADERPARAM_VISIBLE_LIGHTINDICES);


				if (lightGridRWB != nullptr && lightGridRWB->IsValid() &&
					visibleLightIndicesRWB != nullptr && visibleLightIndicesRWB->IsValid())
				{
					mLightGridRBB->Read(lightGridRWB, [&]()
					{
						mLightGridRBB->GetData(LightGrids.data(), sizeof(Graphics::LightGrid) * PreRenderProcess_ComputeCluster::NUM_CLUSTER);
						++mCompeleteRBBCount;
					});
					mVisibleLightIndicesRBB->Read(visibleLightIndicesRWB, [&]()
					{
						mVisibleLightIndicesRBB->GetData(VisibleLightIndices.data(), MAX_VISIBLE_LIGHTINDEX_COUNT * sizeof(u32));
						++mCompeleteRBBCount;
					});
				}
				else
				{
					mEnableDispatch = false;
				}
			}
		}

		if (mCompeleteRBBCount == NUM_RBB_COUNT)
		{
			mEnableDispatch = true;
			mCompeleteRBBCount = 0;
		}
	}
	bool PreRenderProcess_LightCulling::IsCompelete()
	{
		return true;
	}
	Type PreRenderProcess_LightCulling::GetType() const
	{
		return JGTYPE(PreRenderProcess_LightCulling);
	}
	void PreRenderProcess_LightCulling::SetClusters(const void* data, u64 dataCount, u64 dataSize)
	{
		if (mComputer == nullptr) return;

		mComputer->SetStructDataArray(SHADERPARAM_CLUSTERS, data, dataCount, dataSize);
	}
}