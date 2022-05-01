﻿#include "pch.h"
#include "Renderer.h"
#include "Application.h"
#include "JGGraphics.h"
#include "Graphics/RenderBatch.h"
#include "Graphics/Batch/Render2DBatch.h"
#include "Graphics/RenderProcess.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"
#include "PreRenderProcess/PreRenderProcess_ComputeCluster.h"
namespace JG
{
	RenderStatistics Renderer::Statistics;
	Renderer::Renderer()
	{
		mRenderParamManager = CreateUniquePtr<RenderParamManager>();
		mPassData		    = CreateSharedPtr< Graphics::RenderPassData>();
		InitRootSignature();

		mRender2DBatch = CreateBatch<Render2DBatch>();
	}
	bool Renderer::Begin(const RenderInfo& info, List<SharedPtr<Graphics::Light>> lightList)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");


		u64 buffCount = api->GetBufferCount();
		// Context 초기화
		mGraphicsContext = api->GetGraphicsContext();
		mComputeContext  = mGraphicsContext->QueryInterfaceAsComputeContext();
		mCopyContext	 = mGraphicsContext->QueryInterfaceAsCopyContext();
		mRenderInfo		 = info;

		for (auto& _pair : mLightInfos)
		{
			_pair.second.Count = 0;
		}

		// Light Info 정보 수집 <- 나중에 라이트 매니저로 빼기
		// Structured Buffer 사이즈 조절 고민
		for (auto item : lightList)
		{
			Graphics::ELightType lightType = item->GetLightType();
			auto& info = mLightInfos[item->GetLightType()];
			info.Count++;
			info.Size = item->GetBtSize();
			item->PushBtData(info.Data);
		}

		u64 bufferIndex = JGGraphics::GetInstance().GetBufferIndex();
		// PassData  바인딩
		// Light 정보 바인딩
		Graphics::RenderPassData passData;
		passData.ViewMatrix		= JMatrix::Transpose(info.ViewMatrix);
		passData.ProjMatrix     = JMatrix::Transpose(info.ProjMatrix);
		passData.InvViewMatrix = JMatrix::Transpose(JMatrix::Inverse(info.ViewMatrix));
		passData.InvProjMatrix = JMatrix::Transpose(JMatrix::Inverse(info.ProjMatrix));
		passData.ViewProjMatrix = JMatrix::Transpose(info.ViewProjMatrix);
		passData.InvViewProjMatrix = JMatrix::Transpose(JMatrix::Inverse(info.ViewProjMatrix));
		passData.EyePosition	= info.EyePosition;
		passData.FarZ			= info.FarZ;
		passData.NearZ			= info.NearZ;
		passData.Resolution		= info.Resolution;
		passData.PointLightCount = mLightInfos[Graphics::ELightType::PointLight].Count;
		passData.DirectionalLightCount = mLightInfos[Graphics::ELightType::DirectionalLight].Count;
		passData.SpotLightCount = mLightInfos[Graphics::ELightType::SpotLight].Count;
		passData.FrameCount = mFrameCount++;

		ReadyImpl( &passData);

		IRenderProcess::ReadyData readyData;
		readyData.pRenderer = this;
		readyData.GraphicsContext = mGraphicsContext;
		readyData.ComputeContext = mComputeContext;
		readyData.pRenderPassData = &passData;
		readyData.Info = mRenderInfo;
		for (auto& preProcess : mPreProcessList)
		{
			preProcess->Ready(readyData);
		}
		for (auto& postProcess : mPostProcessList)
		{
			postProcess->Ready(readyData);
		}




		SharedPtr<IGraphicsContext> context = api->GetGraphicsContext();
		context->BindConstantBuffer((u32)ERootParam::PassCB, passData);

		(*mPassData) = passData;

		const LightInfo& lInfo = mLightInfos[Graphics::ELightType::PointLight];
		context->BindSturcturedBuffer((u32)ERootParam::PointLight, lInfo.Data.data(), lInfo.Size, lInfo.Count);

		return BeginBatch(info);
	}
	void Renderer::DrawCall(SharedPtr<Graphics::SceneObject> sceneObject)
	{
		if (sceneObject == nullptr)
		{
			return;
		}
		Statistics.TotalObjectCount += 1;


		switch(sceneObject->GetSceneObjectType())
		{
		case Graphics::ESceneObjectType::Paper:
		{
			Graphics::PaperObject* paperObj = static_cast<Graphics::PaperObject*>(sceneObject.get());
			PaperInfo info;
			info.WorldMatrix = paperObj->WorldMatrix;
			info.Color = paperObj->Color;
			if (paperObj->Texture == nullptr || paperObj->Texture->IsValid() == false)
			{
				info.Texture = ITexture::NullTexture();
			}
			else
			{
				info.Texture = paperObj->Texture;
			}
			mPaperInfoList.push_back(info);
			Statistics.VisibleObjectCount += 1;
		}

			break;
		case Graphics::ESceneObjectType::Static:
		{
			Graphics::StaticRenderObject* staticObj = static_cast<Graphics::StaticRenderObject*>(sceneObject.get());
			if (staticObj->Mesh != nullptr && staticObj->MaterialList.empty() == false)
			{
				ObjectInfo info;
				info.WorldMatrix = staticObj->WorldMatrix;
				info.Mesh = staticObj->Mesh;
				info.MaterialList = staticObj->MaterialList;
				info.Flags = staticObj->Flags;
				i32 type = ArrangeObject(info);
				mObjectInfoListDic[type].push_back(info);
				Statistics.VisibleObjectCount += 1;
			}
		}
			break;
		case Graphics::ESceneObjectType::Skeletal:

			break;
		}
	
	}
	SharedPtr<RenderResult> Renderer::End()
	{
		auto api  = JGGraphics::GetInstance().GetGraphicsAPI();
		SharedPtr<RenderResult> result = CreateSharedPtr<RenderResult>();

		// PreProcess Run
		IRenderProcess::RunData runData;
		runData.pRenderer = this;
		runData.GraphicsContext = mGraphicsContext;
		runData.ComputeContext	= mComputeContext;
		runData.Info			= mRenderInfo;
		runData.Result			= result;


		for (auto& preProcess : mPreProcessList)
		{
			preProcess->Run(runData);
		}

	
		while (true)
		{
			bool isCompelete = true;
			for (auto& preProcess : mPreProcessList)
			{
				if (preProcess->IsCompelete() == false)
				{
					isCompelete = false;
				}
			}
			if (isCompelete) break;
		}

		RenderImpl(result);



		// PostProcess
		for (auto& postProcess : mPostProcessList)
		{
			postProcess->Run(runData);
		}


		while (true)
		{
			bool isCompelete = true;
			for (auto& postProcess : mPostProcessList)
			{
				if (postProcess->IsCompelete() == false)
				{
					isCompelete = false;
				}
			}
			if (isCompelete) break;
		}



		CompeleteImpl(result);




		mRenderResult = result;

		// Paper 처리
		if (mRender2DBatch != nullptr)
		{
			for (PaperInfo& info : mPaperInfoList)
			{
				mRender2DBatch->DrawCall(info.WorldMatrix, info.Texture, info.Color);
			}
		}
		EndBatch();
		


		mPaperInfoList.clear();
		mObjectInfoListDic.clear();
		mLightInfos.clear();
		return result;
	}

	void Renderer::SetLightGrid(SharedPtr<IStructuredBuffer> sb)
	{
		mLightGrid = sb;
	}

	void Renderer::SetVisibleLightIndicies(SharedPtr<IStructuredBuffer> sb)
	{
		mVisibleLightIndicies = sb;
	}

	SharedPtr<IStructuredBuffer> Renderer::GetLightGrid() const
	{
		return mLightGrid;
	}

	SharedPtr<IStructuredBuffer> Renderer::GetVisibleLightIndicies() const
	{
		return mVisibleLightIndicies;
	}

	bool Renderer::BeginBatch(const RenderInfo& info)
	{
		bool result = true;
		for (auto& batch : mBatchList)
		{
			if (batch->Begin(info) == false)
			{
				result = false;
			}
		}
		return result;
	}

	void Renderer::EndBatch()
	{
		for (auto& batch : mBatchList)
		{
			batch->End();
		}
	}

	SharedPtr<IGraphicsContext> Renderer::GetGraphicsContext() const
	{
		return mGraphicsContext;
	}
	SharedPtr<IComputeContext> Renderer::GetComputeContext() const
	{
		return mComputeContext;
	}
	SharedPtr<ICopyContext> Renderer::GetCopyContext() const
	{
		return mCopyContext;
	}

	SharedPtr<IRootSignature> Renderer::GetGraphicsRootSignature() const
	{
		return mGraphicsRootSignature;
	}

	SharedPtr<IRootSignature> Renderer::GetComputeRootSignature() const
	{
		return mComputeRootSignature;
	}

	RenderParamManager* Renderer::GetRenderParamManager() const
	{
		return mRenderParamManager.get();
	}

	const RenderInfo& Renderer::GetRenderInfo() const
	{
		return mRenderInfo;
	}

	RenderResult* Renderer::GetRenderResult() const
	{
		return mRenderResult.get();
	}

	const Graphics::RenderPassData& Renderer::GetPassData() const
	{
		return *mPassData;
	}

	const Dictionary<Graphics::ELightType, Renderer::LightInfo>& Renderer::GetLightInfos() const
	{
		return mLightInfos;
	}



	const Renderer::LightInfo& Renderer::GetLightInfo(Graphics::ELightType type)
	{
		return mLightInfos[type];
	}



	const SortedDictionary<int, List<Renderer::ObjectInfo>>& Renderer::GetObjectInfoLists() const
	{
		return mObjectInfoListDic;
	}


	void Renderer::ForEach(const std::function<void(Graphics::ELightType, const LightInfo&)>& action)
	{
		if (action == nullptr)
		{
			return;
		}
		for (auto& _pair : mLightInfos)
		{
			action(_pair.first, _pair.second);
		}


	}
	void Renderer::ForEach(const std::function<void(i32, const List<ObjectInfo>&)>& action)
	{
		if (action == nullptr)
		{
			return;
		}
		for (auto& _pair : mObjectInfoListDic)
		{
			action(_pair.first, _pair.second);
		}
	}
	void Renderer::InitRootSignature()
	{
		//Graphics
		{
			auto creater = IRootSignatureCreater::Create();
			creater->AddSRV((u32)ERootParam::PointLight, 0, 3);
			creater->AddCBV((u32)ERootParam::PassCB, 0, 0);
			creater->AddCBV((u32)ERootParam::ObjectCB, 1, 0);
			creater->AddCBV((u32)ERootParam::MaterialCB, 2, 0);
			creater->AddDescriptorTable((u32)ERootParam::Texture2D, EDescriptorTableRangeType::SRV, 1024, 0, 0);
			creater->AddDescriptorTable((u32)ERootParam::TextureCube, EDescriptorTableRangeType::SRV, 1024, 0, 1);
			creater->AddSRV((u32)ERootParam::LightGrid, 0, 11);
			creater->AddSRV((u32)ERootParam::VisibleLightIndicies, 0, 12);
			creater->AddSampler(0, ESamplerFilter::Point, ETextureAddressMode::Wrap);
			creater->AddSampler(1, ESamplerFilter::Linear, ETextureAddressMode::Wrap);
			creater->AddSampler(2, ESamplerFilter::Anisotropic, ETextureAddressMode::Wrap);
			creater->AddSampler(3, ESamplerFilter::Point, ETextureAddressMode::Clamp);
			creater->AddSampler(4, ESamplerFilter::Linear, ETextureAddressMode::Clamp);
			creater->AddSampler(5, ESamplerFilter::Anisotropic, ETextureAddressMode::Clamp);
			creater->AddSampler(6, ESamplerFilter::Linear, ETextureAddressMode::Border);
			mGraphicsRootSignature = creater->Generate();
		}
		// Compute
		{
			auto creater = IRootSignatureCreater::Create();
			creater->AddDescriptorTable((u32)EComputeRootParam::UAV, EDescriptorTableRangeType::UAV, 1024, 0, 0);
			creater->AddDescriptorTable((u32)EComputeRootParam::SRV, EDescriptorTableRangeType::SRV, 1024, 0, 0);
			creater->AddCBV((u32)EComputeRootParam::CB0, 0, 0);
			creater->AddSampler(0, ESamplerFilter::Linear, ETextureAddressMode::Clamp);
			mComputeRootSignature = creater->Generate();
		}


	}
}