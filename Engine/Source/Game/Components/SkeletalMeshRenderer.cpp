#include "pch.h"
#include "SkeletalMeshRenderer.h"
#include "Transform.h"
#include "Common/DragAndDrop.h"
#include "Graphics/JGGraphics.h"
#include "GameWorld.h"
#include "Class/Data/Skeletone.h"
#include "Animation/JGAnimation.h"
#include "Animation/AnimationController.h"
namespace JG
{
	void SkeletalMeshRenderer::Awake()
	{
		BaseRenderer::Awake();

	}
	void SkeletalMeshRenderer::Start()
	{
		BaseRenderer::Start();
		if (mAnimatioin != nullptr && mAnimatioin->IsValid())
		{
			JGAnimation::GetInstance().RegisterAnimationController(mAnimatioin->Get());
			if (mSkeletone != nullptr)
			{
				mAnimatioin->Get()->BindSkeletone(mSkeletone->Get());
			}
			if (mMesh != nullptr)
			{
				mAnimatioin->Get()->BindMesh(mMesh->Get());
			}
		}
	}
	void SkeletalMeshRenderer::Destory()
	{
		BaseRenderer::Destory();
		if (mAnimatioin != nullptr && mAnimatioin->IsValid())
		{
			JGAnimation::GetInstance().UnRegisterAnimatioinController(mAnimatioin->Get());
		}

	}
	void SkeletalMeshRenderer::Update()
	{
		BaseRenderer::Update();

	}
	void SkeletalMeshRenderer::LateUpdate()
	{
		BaseRenderer::LateUpdate();

	}
	void SkeletalMeshRenderer::FixedUpdate()
	{
		BaseRenderer::FixedUpdate();
		PushRenderSceneObject();
	}
	void SkeletalMeshRenderer::SetMesh(const String& path)
	{
		auto assetManager = GetGameWorld()->GetAssetManager();
		if (assetManager->GetAssetFormat(path) != EAssetFormat::SkeletalMesh)
		{
			return;
		}
		mMesh = GetGameWorld()->GetAssetManager()->RequestOriginAsset<IMesh>(path);
	}
	void SkeletalMeshRenderer::SetMaterial(const String& path)
	{
		if (mMaterialList.empty())
		{
			mMaterialList.resize(1);
		}
		mMaterialList[0] = GetGameWorld()->GetAssetManager()->RequestRWAsset<IMaterial>(path);
	}
	void SkeletalMeshRenderer::SetMaterial(i32 slot, const String& path)
	{
		if (slot < 0) return;
		auto cnt = GetMaterialCount();
		if (slot >= cnt)
		{
			return;
		}
		mMaterialList[slot] = GetGameWorld()->GetAssetManager()->RequestRWAsset<IMaterial>(path);
	}
	void SkeletalMeshRenderer::SetSkeletone(const String& path)
	{
		auto assetManager = GetGameWorld()->GetAssetManager();
		if (assetManager->GetAssetFormat(path) != EAssetFormat::Skeletal)
		{
			return;
		}
		mSkeletone = GetGameWorld()->GetAssetManager()->RequestOriginAsset<Skeletone>(path);


	}
	void SkeletalMeshRenderer::SetAnimation(const String& path)
	{
		auto assetManager = GetGameWorld()->GetAssetManager();
		if (assetManager->GetAssetFormat(path) != EAssetFormat::Animation)
		{
			return;
		}
		mAnimatioin = GetGameWorld()->GetAssetManager()->RequestRWAsset<AnimationController>(path);
	}
	void SkeletalMeshRenderer::AddMaterial(const String& path)
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
	void SkeletalMeshRenderer::PopMaterial()
	{
		if (mMaterialList.empty() == false)
		{
			mMaterialList.pop_back();
		}
	}
	i32 SkeletalMeshRenderer::GetMaterialCount() const
	{
		return mMaterialList.size();
	}
	Asset<IMesh>* SkeletalMeshRenderer::GetMesh() const
	{
		return mMesh;
	}
	Asset<IMaterial>* SkeletalMeshRenderer::GetMaterial(i32 slot) const
	{
		if (mMaterialList.size() <= slot)
		{
			return nullptr;
		}
		return mMaterialList[slot];
	}
	Asset<Skeletone>* SkeletalMeshRenderer::GetSkeletone() const
	{
		return mSkeletone;
	}
	Asset<AnimationController>* SkeletalMeshRenderer::GetAnimation() const
	{
		return mAnimatioin;
	}
	void SkeletalMeshRenderer::ForEach(const std::function<void(Asset<IMaterial>*)>& action)
	{
		for (auto& m : mMaterialList)
		{
			action(m);
		}
	}
	void SkeletalMeshRenderer::MakeJson(SharedPtr<JsonData> jsonData) const
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

		
		if (mSkeletone)
		{
			jsonData->AddMember("Skeletone", mSkeletone->GetAssetPath());
		}
		if (mAnimatioin)
		{
			jsonData->AddMember("Animation", mAnimatioin->GetAssetPath());
		}
	}
	void SkeletalMeshRenderer::LoadJson(SharedPtr<JsonData> jsonData)
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
				mMaterialList[i] = GetGameWorld()->GetAssetManager()->RequestRWAsset<IMaterial>(path);
			}
		}
		val = jsonData->GetMember("Skeletone");
		if (val && val->IsString())
		{
			SetSkeletone(val->GetString());
		}
		val = jsonData->GetMember("Animation");
		if (val && val->IsString())
		{
			SetAnimation(val->GetString());
		}
	}
	void SkeletalMeshRenderer::OnChange(const ChangeData& data)
	{
		BaseRenderer::OnChange(data);
	}
	void SkeletalMeshRenderer::PushRenderSceneObject()
	{
		if (mMesh == nullptr || mMesh->IsValid() == false)
		{
			return;
		}


		auto sceneObject = CreateSharedPtr<Graphics::StaticRenderObject>();
		auto transform = GetOwner()->GetTransform();
		sceneObject->WorldMatrix = transform->GetWorldMatrix();

		if (mAnimatioin != nullptr && 
			mAnimatioin->IsValid() &&
			mAnimatioin->Get()->GetBindedMesh() != nullptr &&
			mAnimatioin->Get()->GetBindedSkeletone() != nullptr)
		{
			sceneObject->Mesh = mAnimatioin->Get()->GetBindedMesh();
		}
		else
		{
			sceneObject->Mesh = mMesh->Get();
		}
		
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