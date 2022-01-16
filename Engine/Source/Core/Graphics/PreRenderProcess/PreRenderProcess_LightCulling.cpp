#include "pch.h"
#include "PreRenderProcess_LightCulling.h"
#include "PreRenderProcess_ComputeCluster.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"
namespace JG
{
	void PreRenderProcess_LightCulling::Awake(const AwakeData& data)
	{
		SharedPtr<IRootSignatureCreater> creater = IRootSignatureCreater::Create();

		creater->AddCBV(0, 0, 0);
		creater->AddUAV(1, 0, 0);
		creater->AddUAV(2, 0, 1);
		creater->AddSRV(3, 0, 2);
		creater->AddSRV(4, 0, 3);

		mRootSignature = creater->Generate();
		mShader = ShaderLibrary::GetInstance().FindComputeShader(SHADER_NAME);

		GraphicsHelper::InitStrucutredBuffer("LightGridSB", PreRenderProcess_ComputeCluster::NUM_CLUSTER, sizeof(Graphics::LightGrid), &mLightGridSB);
		GraphicsHelper::InitStrucutredBuffer("VisibleLightIndiciesSB", PreRenderProcess_ComputeCluster::NUM_CLUSTER * 64, sizeof(u32), &mVisibleLightIndiciesSB);
	}
	void PreRenderProcess_LightCulling::Ready(const ReadyData& data)
	{
		CB.ViewMatirx      = JMatrix::Transpose(data.Info.ViewMatrix);
		CB.PointLightCount = data.pRenderer->GetLightInfo(Graphics::ELightType::PointLight).Count;
		CB.NumXSlice = PreRenderProcess_ComputeCluster::NUM_X_SLICE;
		CB.NumYSlice = PreRenderProcess_ComputeCluster::NUM_Y_SLICE;
		CB.NumZSlice = PreRenderProcess_ComputeCluster::NUM_Z_SLICE;


		SharedPtr<IGraphicsContext> context = data.GraphicsContext;
		context->BindSturcturedBuffer(Renderer::RootParam_LightGrid, mLightGridSB[data.Info.CurrentBufferIndex]);
		context->BindSturcturedBuffer(Renderer::RootParam_VisibleLightIndicies, mVisibleLightIndiciesSB[data.Info.CurrentBufferIndex]);
	}
	void PreRenderProcess_LightCulling::Run(const RunData& data)
	{
		auto pointLightsInfo = data.pRenderer->GetLightInfo(Graphics::ELightType::PointLight);


		SharedPtr<IStructuredBuffer> targetLightGridSB = mLightGridSB[data.Info.CurrentBufferIndex];
		SharedPtr<IStructuredBuffer> targetVisibleLightIndiciesSB = mVisibleLightIndiciesSB[data.Info.CurrentBufferIndex];



		SharedPtr<IComputeContext> context = data.ComputeContext;

		context->BindRootSignature(mRootSignature);
		context->BindShader(mShader);
		context->BindConstantBuffer(0, CB); 
		context->BindSturcturedBuffer(1, targetVisibleLightIndiciesSB);
		context->BindSturcturedBuffer(2, targetLightGridSB);
		context->BindSturcturedBuffer(3, pointLightsInfo.SB[data.Info.CurrentBufferIndex]);
		context->BindSturcturedBuffer(4, mClusterSB);
		context->Dispatch(
			PreRenderProcess_ComputeCluster::NUM_X_SLICE, 
			PreRenderProcess_ComputeCluster::NUM_Y_SLICE, 
			PreRenderProcess_ComputeCluster::NUM_Z_SLICE);
	}
	bool PreRenderProcess_LightCulling::IsCompelete()
	{
		return true;
	}
	Type PreRenderProcess_LightCulling::GetType() const
	{
		return JGTYPE(PreRenderProcess_LightCulling);
	}
	void PreRenderProcess_LightCulling::SetClusters(SharedPtr<IStructuredBuffer> sb)
	{
		mClusterSB = sb;
	}
}