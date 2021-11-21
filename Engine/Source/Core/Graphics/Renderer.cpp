#include "pch.h"
#include "Renderer.h"
#include "Application.h"
#include "JGGraphics.h"
#include "Graphics/RenderBatch.h"

namespace JG
{
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
		for (auto item : lightList)
		{
			auto& info = mLightInfos[item->GetLightType()];
			info.Count++;
			info.Size = item->GetBtSize();
			item->PushBtData(info.ByteData);
		}
		return BeginBatch(info, batchList);
	}
	void Renderer::DrawCall(const JMatrix& worldMatrix, SharedPtr<IMesh> mesh, List<SharedPtr<IMaterial>> materialList)
	{
		if (mesh == nullptr || materialList.empty())
		{
			return;
		}
		ObjectInfo info;
		info.WorldMatrix = worldMatrix;
		info.Mesh = mesh;
		info.MaterialList = materialList;
		i32 type = ArrangeObject(info);
		mObjectInfoListDic[type].push_back(info);
	}
	void Renderer::End()
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		auto cnt = mObjectDrawFuncList.size();
		for (i32 i = 0; i < cnt; ++i)
		{
			if (mReadyDrawFuncList[i] != nullptr)
			{
				mReadyDrawFuncList[i](api, mCurrentRenderInfo);
			}
		
			if (mObjectDrawFuncList[i] != nullptr)
			{
				for (auto& _pair : mObjectInfoListDic)
				{
					mObjectDrawFuncList[i](_pair.first, _pair.second);
				}
			}
			if (mSceneDrawFuncList[i] != nullptr)
			{
				mSceneDrawFuncList[i]();
			}
		}
		mObjectInfoListDic.clear();
		EndBatch();
	}

	u64 Renderer::GetCommandID() const
	{
		return mCurrentRenderInfo.CommandID;
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

	void Renderer::AddDrawFunc(const ReadyDrawFunc& readyFunc, const ObjectDrawFunc& drawObjectFunc, const SceneDrawFunc& sceneDrawFunc)
	{
		mReadyDrawFuncList.push_back(readyFunc);
		mObjectDrawFuncList.push_back(drawObjectFunc);
		mSceneDrawFuncList.push_back(sceneDrawFunc);
	}
}