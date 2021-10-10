#include "pch.h"
#include "StaticMeshRenderer.h"
#include "Transform.h"
#include "Common/DragAndDrop.h"
#include "Graphics/Resource.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Mesh.h"
#include "Class/Game/GameWorld.h"


namespace JG
{
	StaticMeshRenderer::StaticMeshRenderer()
	{
		mStaticRI = CreateSharedPtr<StandardStaticMeshRenderItem>();
	}
	void StaticMeshRenderer::Awake()
	{
	}
	void StaticMeshRenderer::Start()
	{

	}
	void StaticMeshRenderer::Destory()
	{

	}
	void StaticMeshRenderer::Update()
	{

	}
	void StaticMeshRenderer::LateUpdate()
	{
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
		mMaterialList.push_back(GetGameWorld()->GetAssetManager()->RequestRWAsset<IMaterial>(path));
	}
	i32 StaticMeshRenderer::GetMaterialCount() const
	{
		return mMaterialList.size();
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
	SharedPtr<IRenderItem> StaticMeshRenderer::PushRenderItem()
	{
		auto transform = GetOwner()->GetTransform();
		mStaticRI->WorldMatrix = transform->GetWorldMatrix();

		if (mMesh && mMesh->Get() && mMesh->Get()->IsValid())
		{
			mStaticRI->Mesh = mMesh->Get();
			GetOwner()->SetPickingBoundingBox(mStaticRI->Mesh->GetBoundingBox());
		}

		auto matAssetCnt = mMaterialList.size();
		mStaticRI->Materials.resize(matAssetCnt);


		for (u64 i = 0; i < matAssetCnt; ++i)
		{
			auto material = mMaterialList[i];
			if (material == nullptr)
			{
				mStaticRI->Materials[i] = IMaterial::Create("NullMaterial", ShaderLibrary::GetInstance().GetShader(ShaderScript::Template::Standard3DShader));
			}
			else
			{
				mStaticRI->Materials[i] = material->Get();
			}
		}


		if (matAssetCnt == 0 && mStaticRI->Materials.empty()) {
			if (mNullMaterial == nullptr)
			{
				mNullMaterial = IMaterial::Create("NullMaterial", ShaderLibrary::GetInstance().GetShader(ShaderScript::Template::Standard3DShader));
			}
			mStaticRI->Materials.push_back(mNullMaterial);
		}
		return mStaticRI;
	}
	void StaticMeshRenderer::OnChange(const ChangeData& data)
	{
		BaseRenderer::OnChange(data);
	}
	void StaticMeshRenderer::OnInspectorGUI()
	{
		BaseRenderer::OnInspectorGUI();




		f32 label_width = ImGui::CalcTextSize("Materials").x;



		String inputText = "None";
		if (mMesh)
		{
			inputText = mMesh->GetAssetName();
		}
		ImGui::AssetField_OnGUI("Mesh", inputText, EAssetFormat::Mesh, [&](const std::string& path)
		{
			SetMesh(path);
		}, label_width);







		List<String> inputTextList;

		for (auto& m : mMaterialList)
		{
			if (m == nullptr) {
				inputTextList.push_back("None");
			}
			else
			{
				inputTextList.push_back(m->GetAssetName());
			}

	
		}
		ImGui::AssetField_List_OnGUI("Materials", inputTextList, EAssetFormat::Material,
			[&](int index, const std::string& path)
		{
			mMaterialList[index] = GetGameWorld()->GetAssetManager()->RequestRWAsset<IMaterial>(path);
		}, [&]()
		{
			mMaterialList.push_back(nullptr);
		}, [&]()
		{
			mMaterialList.pop_back();
		}, label_width);
	}
	void StaticMeshRenderer::OnInspector_MeshGUI()
	{
	}
	void StaticMeshRenderer::OnInspector_MaterialGUI()
	{
	}
}