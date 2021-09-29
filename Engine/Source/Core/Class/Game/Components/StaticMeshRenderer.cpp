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
		//OnInspector_MeshGUI();
		//OnInspector_MaterialGUI();
	}
	void StaticMeshRenderer::OnInspector_MeshGUI()
	{
		//String out;
		//String in = "None";
		//if (mMesh)
		//{
		//	in = mMesh->GetAssetName();
		//}
		//if (ImGui::AssetField("Mesh    ", in, EAssetFormat::Mesh, out))
		//{
		//	mMesh = GetGameWorld()->GetAssetManager()->RequestOriginAsset<IMesh>(out);
		//}
	}
	void StaticMeshRenderer::OnInspector_MaterialGUI()
	{




		//auto cnt = mMaterialList.size();
		//bool isOpenTree = ImGui::TreeNodeEx(("Material : " + std::to_string(cnt)).c_str(), ImGuiTreeNodeFlags_DefaultOpen);
		//ImGui::SameLine();
		//if (ImGui::SmallButton("+"))
		//{
		//	mMaterialList.push_back(nullptr);
		//}ImGui::SameLine();
		//if (ImGui::SmallButton("-"))
		//{
		//	if (mMaterialList.empty() == false)
		//	{
		//		mMaterialList.pop_back();
		//	}

		//}


		//if (isOpenTree)
		//{
		//	cnt = mMaterialList.size();
		//	for (i32 i = 0; i < cnt; ++i)
		//	{
		//		String out;
		//		String in = "None";
		//		if (mMaterialList[i])
		//		{
		//			in = mMaterialList[i]->GetAssetName();
		//		}

		//		if (ImGui::AssetField("Slot " + std::to_string(i), in, EAssetFormat::Material, out))
		//		{
		//			mMaterialList[i] = GetGameWorld()->GetAssetManager()->RequestRWAsset<IMaterial>(out);
		//		}
		//	}
		//	ImGui::TreePop();
		//}
	}
}