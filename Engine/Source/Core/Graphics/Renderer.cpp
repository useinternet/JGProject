#include "pch.h"
#include "Renderer.h"
#include "Application.h"
#include "JGGraphics.h"
#include "Graphics/RenderBatch.h"
#include "Graphics/RenderProcess.h"
#include "PreRenderProcess/PreRenderProcess_ComputeCluster.h"
namespace JG
{
	RenderStatistics Renderer::Statistics;
	bool Renderer::Begin(const RenderInfo& info, List<SharedPtr<Graphics::Light>> lightList, List<SharedPtr<RenderBatch>> batchList)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		mCurrentRenderInfo = info;
		if (info.TargetTexture == nullptr)
		{
			return false;
		}
		mLightInfos.clear();


		// Light Info 정보 수집
		u64 pl_count = 0;
		for (auto item : lightList)
		{
			auto& info = mLightInfos[item->GetLightType()];
			info.Count++;
			info.Size = item->GetBtSize();
			item->PushBtData(info.ByteData);

			switch (item->GetLightType())
			{
			case Graphics::ELightType::PointLight:
				++pl_count;
				break;
			}
		}

		auto commandID = JGGraphics::GetInstance().RequestCommandID();
		api->BeginDraw(commandID);

		// PassData  바인딩
		// Light 정보 바인딩
		Graphics::RenderPassData passData;
		passData.ViewMatrix		= JMatrix::Transpose(info.ViewMatrix);
		passData.ProjMatrix     = JMatrix::Transpose(info.ProjMatrix);
		passData.ViewProjMatrix = JMatrix::Transpose(info.ViewProjMatrix);
		passData.EyePosition	= info.EyePosition;
		passData.FarZ			= info.FarZ;
		passData.NearZ			= info.NearZ;
		passData.Resolution		= info.Resolutoin;
		passData.PointLightCount = pl_count;



		ReadyImpl(api, &passData, info);
		for (auto& preProcess : mPreProcessList)
		{
			preProcess->Ready(this, api, &passData, mCurrentRenderInfo);
		}
		for (auto& postProcess : mPostProcessList)
		{
			postProcess->Ready(this, api, &passData, mCurrentRenderInfo);
		}


		api->SetRenderPassData(commandID, passData);
		api->SetLights(commandID, lightList);
		return BeginBatch(info, batchList);
	}
	void Renderer::DrawCall(const JMatrix& worldMatrix, SharedPtr<IMesh> mesh, List<SharedPtr<IMaterial>> materialList)
	{
		if (mesh == nullptr || materialList.empty())
		{
			return;
		}
		Statistics.TotalObjectCount += 1;
		// Culling


		ObjectInfo info;
		info.WorldMatrix = worldMatrix;
		info.Mesh = mesh;
		info.MaterialList = materialList;
		i32 type = ArrangeObject(info);
		mObjectInfoListDic[type].push_back(info);
		
		Statistics.VisibleObjectCount += 1;
	}
	void Renderer::End()
	{
		auto api  = JGGraphics::GetInstance().GetGraphicsAPI();
		auto info = GetRenderInfo();
	

		// PreProcess Run
		for (auto& preProcess : mPreProcessList)
		{
			preProcess->Run(this, api, mCurrentRenderInfo);
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

		// Render
		RenderImpl(api, mCurrentRenderInfo);



		// PostProcess
		for (auto& postProcess : mPostProcessList)
		{
			postProcess->Run(this, api, mCurrentRenderInfo);
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






		mObjectInfoListDic.clear();
		EndBatch();

		CompeleteImpl(api, info);

		auto commandID = JGGraphics::GetInstance().RequestCommandID();
		api->EndDraw(commandID);
	}

	bool Renderer::BeginBatch(const RenderInfo& info, List<SharedPtr<RenderBatch>> batchList)
	{
		bool result = true;
		mBatchList = batchList;
		if (mBatchList.empty() == false)
		{
			for (auto& batch : mBatchList)
			{
				batch->ConnectRenderer(this);
				if (batch->Begin(info) == false)
				{
					result = false;
				}
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
		mBatchList.clear();
	}



	const RenderInfo& Renderer::GetRenderInfo() const
	{
		return mCurrentRenderInfo;
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
}