#include "pch.h"
#include "PreRenderProcess_LightCulling.h"
#include "PreRenderProcess_ComputeCluster.h"

#include "Graphics/GraphicsHelper.h"
namespace JG
{
	PreRenderProcess_LightCulling::PreRenderProcess_LightCulling()
	{

	}
	void PreRenderProcess_LightCulling::Awake(Renderer* renderer)
	{

	}
	void PreRenderProcess_LightCulling::Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* renderPassData, const RenderInfo& info)
	{
		InitComputers();
		CB.ViewMatirx      = JMatrix::Transpose(info.ViewMatrix);
		CB.PointLightCount = renderer->GetLightInfo(Graphics::ELightType::PointLight).Count;
	}
	void PreRenderProcess_LightCulling::Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{

		auto pointLightsInfo = renderer->GetLightInfo(Graphics::ELightType::PointLight);



		SharedPtr<IComputer>         targetComputer = mLightCullingComputers[info.CurrentBufferIndex];
		SharedPtr<IStructuredBuffer> targetLightSB = mLightSB[info.CurrentBufferIndex];
		SharedPtr<IStructuredBuffer> targetLightGridSB = mLightGridSB[info.CurrentBufferIndex];
		SharedPtr<IStructuredBuffer> targetVisibleLightIndiciesSB = mVisibleLightIndiciesSB[info.CurrentBufferIndex];




		targetLightSB->SetData(pointLightsInfo.Size, pointLightsInfo.Count, pointLightsInfo.ByteData.data());
		api->SetLightGrids(targetLightGridSB);
		api->SetVisibleLightIndicies(targetVisibleLightIndiciesSB);

		targetComputer->SetFloat4x4(SHADERPARAM_VIEWMATRIX, CB.ViewMatirx);
		targetComputer->SetInt(SHADERPARAM_POINTLIGHTCOUNT, CB.PointLightCount);
		targetComputer->Dispatch(PreRenderProcess_ComputeCluster::NUM_X_SLICE, PreRenderProcess_ComputeCluster::NUM_Y_SLICE, PreRenderProcess_ComputeCluster::NUM_Z_SLICE, false);
	}
	bool PreRenderProcess_LightCulling::IsCompelete()
	{
		return true;
	}
	Type PreRenderProcess_LightCulling::GetType() const
	{
		return JGTYPE(PreRenderProcess_LightCulling);
	}
	bool PreRenderProcess_LightCulling::InitComputers()
	{
		if (mLightCullingComputers.empty() == false)
		{
			return true;
		}
	
		auto shader = ShaderLibrary::GetInstance().FindComputeShader(SHADER_NAME);
		if (shader == nullptr)
		{
			return false;
		}


		GraphicsHelper::InitComputer("LightCulling_Computer", shader, &mLightCullingComputers);
		GraphicsHelper::InitStrucutredBuffer("LightSB", 1024, 48, &mLightSB);
		GraphicsHelper::InitStrucutredBuffer("LightGridSB", PreRenderProcess_ComputeCluster::NUM_CLUSTER, sizeof(Graphics::LightGrid), &mLightGridSB);
		GraphicsHelper::InitStrucutredBuffer("VisibleLightIndiciesSB", PreRenderProcess_ComputeCluster::NUM_CLUSTER * 64, sizeof(u32), &mVisibleLightIndiciesSB);


		i32 index = 0;
		for (auto& computer : mLightCullingComputers)
		{
			computer->SetUint(SHADERPARAM_NUM_X_SLICE, PreRenderProcess_ComputeCluster::NUM_X_SLICE);
			computer->SetUint(SHADERPARAM_NUM_Y_SLICE, PreRenderProcess_ComputeCluster::NUM_Y_SLICE);
			computer->SetUint(SHADERPARAM_NUM_Z_SLICE, PreRenderProcess_ComputeCluster::NUM_Z_SLICE);
			computer->SetStructuredBuffer(SHADERPARAM_POINTLIGHTS, mLightSB[index]);
			computer->SetStructuredBuffer(SHADERPARAM_LIGHTGRIDS,  mLightGridSB[index]);
			computer->SetStructuredBuffer(SHADERPARAM_VISIBLE_LIGHTINDICES, mVisibleLightIndiciesSB[index]);
			index++;
		}
		return true;
	}
	void PreRenderProcess_LightCulling::SetClusters(SharedPtr<IStructuredBuffer> sb)
	{
		for (auto& computer : mLightCullingComputers)
		{
			computer->SetStructuredBuffer(SHADERPARAM_CLUSTERS, sb);
		}
	}
}