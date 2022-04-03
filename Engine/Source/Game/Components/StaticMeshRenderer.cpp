#include "pch.h"
#include "StaticMeshRenderer.h"
#include "Transform.h"
#include "Common/DragAndDrop.h"
#include "Graphics/JGGraphics.h"
#include "GameWorld.h"


namespace JG
{
	void StaticMeshRenderer::Awake()
	{
		BaseRenderer::Awake();
	
	}
	void StaticMeshRenderer::Start()
	{
		BaseRenderer::Start();

	}
	void StaticMeshRenderer::Destory()
	{
		BaseRenderer::Destory();

	}
	void StaticMeshRenderer::Update()
	{
		BaseRenderer::Update();
	
	}
	void StaticMeshRenderer::LateUpdate()
	{
		BaseRenderer::LateUpdate();

	}
	void StaticMeshRenderer::FixedUpdate()
	{
		BaseRenderer::FixedUpdate();
		PushRenderSceneObject();
	}
	void StaticMeshRenderer::SetMesh(const String& path)
	{
		auto assetManager = GetGameWorld()->GetAssetManager();
		if (assetManager->GetAssetFormat(path) != EAssetFormat::Mesh)
		{
			return;
		}
		mMesh = GetGameWorld()->GetAssetManager()->RequestOriginAsset<IMesh>(path);
	}
	void StaticMeshRenderer::SetMaterial(const String& path)
	{
		if (mMaterialList.empty())
		{
			mMaterialList.resize(1);
		}
		mMaterialList[0] = GetGameWorld()->GetAssetManager()->RequestRWAsset<IMaterial>(path);
	}
	void StaticMeshRenderer::SetMaterial(i32 slot, const String& path)
	{
		if (slot < 0) return;
		auto cnt = GetMaterialCount();
		if (slot >= cnt)
		{
			return;
		}
		mMaterialList[slot] = GetGameWorld()->GetAssetManager()->RequestRWAsset<IMaterial>(path);
	}
	void StaticMeshRenderer::AddMaterial(const String& path)
	{
		if (path.empty())
		{
			mMaterialList.push_back(nullptr);

		}
		else
		{
			mMaterialList.push_back(GetGameWorld()->GetAssetManager()->RequestRWAsset<IMaterial>(path));
		}
	
	}
	void StaticMeshRenderer::PopMaterial()
	{
		if (mMaterialList.empty() == false)
		{
			mMaterialList.pop_back();
		}
	}
	i32 StaticMeshRenderer::GetMaterialCount() const
	{
		return mMaterialList.size();
	}
	Asset<IMesh>* StaticMeshRenderer::GetMesh() const
	{
		return mMesh;
	}
	Asset<IMaterial>* StaticMeshRenderer::GetMaterial(i32 slot) const
	{
		if (mMaterialList.size() <= slot)
		{
			return nullptr;
		}
		return mMaterialList[slot];
	}
	void StaticMeshRenderer::ForEach(const std::function<void(Asset<IMaterial>*)>& action)
	{
		for (auto& m : mMaterialList)
		{
			action(m);
		}
	}
	void StaticMeshRenderer::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		BaseRenderer::MakeJson(jsonData);
		if (mMesh)
		{
			jsonData->AddMember("MeshPath", mMesh->GetAssetPath());
		}


		auto materialListJson = jsonData->CreateJsonData();
		for (auto& material : mMaterialList)
		{
			if (material == nullptr) continue;
			auto materialJson = jsonData->CreateJsonData();

			materialJson->SetString(material->GetAssetPath());
			materialListJson->AddMember(materialJson);
		}
		auto type = materialListJson->GetValue().GetType();
		jsonData->AddMember("Materials", materialListJson);

	}
	void StaticMeshRenderer::LoadJson(SharedPtr<JsonData> jsonData)
	{
		BaseRenderer::LoadJson(jsonData);
		auto val = jsonData->GetMember("MeshPath");
		if (val && val->IsString())
		{
			SetMesh(val->GetString());
		}
		val = jsonData->GetMember("Materials");
		if (val && val->IsArray())
		{
			auto type = val->GetValue().GetType();
			mMaterialList.resize(val->GetSize());
			for (i32 i = 0; i < val->GetSize(); ++i)
			{
				auto material = val->GetJsonDataFromIndex(i);
				auto path = material->GetString();
				mMaterialList[i] =  GetGameWorld()->GetAssetManager()->RequestRWAsset<IMaterial>(path);
			}
		}
	}
	void StaticMeshRenderer::OnChange(const ChangeData& data)
	{
		BaseRenderer::OnChange(data);
	}
	void StaticMeshRenderer::OnInspector_MeshGUI()
	{
	}
	void StaticMeshRenderer::OnInspector_MaterialGUI()
	{
	}
	void StaticMeshRenderer::PushRenderSceneObject()
	{
		if (mMesh == nullptr || mMesh->IsValid() == false)
		{
			return;
		}


		auto sceneObject = CreateSharedPtr<Graphics::StaticRenderObject>();
		auto transform = GetOwner()->GetTransform();
		sceneObject->WorldMatrix = transform->GetWorldMatrix();

		sceneObject->Mesh = mMesh->Get();
		GetOwner()->SetPickingBoundingBox(sceneObject->Mesh->GetBoundingBox());



		auto matAssetCnt = mMaterialList.size();
		sceneObject->MaterialList.resize(matAssetCnt);


		for (u64 i = 0; i < matAssetCnt; ++i)
		{
			auto material = mMaterialList[i];
			if (material != nullptr)
			{
				sceneObject->MaterialList[i] = material->Get();
			}
		}
		GetGameWorld()->PushRenderSceneObject(sceneObject);
	}
}