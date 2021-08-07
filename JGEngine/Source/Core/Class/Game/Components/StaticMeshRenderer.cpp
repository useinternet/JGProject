#include "pch.h"
#include "StaticMeshRenderer.h"
#include "Transform.h"
#include "Common/DragAndDrop.h"
#include "Graphics/Resource.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Mesh.h"
#include "Class/UI/UIViewModel/ContentsViewModel.h"
#include "Class/Game/GameWorld.h"


namespace JG
{
	StaticMeshRenderer::StaticMeshRenderer()
	{
		mStaticRI = CreateSharedPtr<StandardStaticMeshRenderItem>();
		// Material Asset
		//mStaticRI->Materials.resize(1);
		//mStaticRI->Materials[0] = IMaterial::Create("DefaultMaterial", ShaderLibrary::GetInstance().GetShader(ShaderScript::Standard3DShader, { "StandardScript" }));
		//mStaticRI->Materials[0]->SetFloat4("TestColor", JVector4(1.0f, 0.0f, 0.0f, 1.0f));
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
		mMeshAssetHandle = GetGameWorld()->GetAssetManager()->RequestOriginAsset<IMesh>(path);
	}
	void StaticMeshRenderer::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		BaseRenderer::MakeJson(jsonData);
		if (mMeshAssetHandle && mMeshAssetHandle->IsValid())
		{
			jsonData->AddMember("MeshPath", mMeshAssetHandle->GetAsset()->GetAssetPath());
		}


		auto materialListJson = jsonData->CreateJsonData();
		for (auto& material : mMaterialAssetHandleList)
		{
			if (material == nullptr || material->IsValid() == false) continue;
			auto materialJson = jsonData->CreateJsonData();

			materialJson->SetString(material->GetAsset()->GetAssetPath());
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
			mMaterialAssetHandleList.resize(val->GetSize());
			for (i32 i = 0; i < val->GetSize(); ++i)
			{
				auto material = val->GetJsonDataFromIndex(i);
				auto path = material->GetString();

				mMaterialAssetHandleList[i] = GetGameWorld()->GetAssetManager()->RequestOriginAsset<IMaterial>(path);

			}
		}
	}
	SharedPtr<IRenderItem> StaticMeshRenderer::PushRenderItem()
	{
		auto transform = GetOwner()->GetTransform();
		mStaticRI->WorldMatrix = transform->GetWorldMatrix();

		if (mMeshAssetHandle && mMeshAssetHandle->IsValid())
		{
			mStaticRI->Mesh = mMeshAssetHandle->GetAsset()->Get();
			GetOwner()->SetBoundingBox(mStaticRI->Mesh->GetBoundingBox());
		}

		auto matAssetCnt = mMaterialAssetHandleList.size();
		for (u64 i = 0; i < matAssetCnt; ++i)
		{
			auto material = mMaterialAssetHandleList[i];
			if (material == nullptr || material->IsValid() == false) continue;

			if (mStaticRI->Materials.size() > i) {
				mStaticRI->Materials[i] = material->GetAsset()->Get();
			}
			else
			{
				mStaticRI->Materials.push_back(material->GetAsset()->Get());
			}
		}
		mStaticRI->Materials.resize(matAssetCnt);
		if (matAssetCnt == 0 && mStaticRI->Materials.empty()) {
			if (mNullMaterial == nullptr)
			{
				mNullMaterial = IMaterial::Create("DefaultMaterial", ShaderLibrary::GetInstance().GetShader(ShaderScript::Template::Standard3DShader));
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
		OnInspector_MeshGUI();
		OnInspector_MaterialGUI();
	}
	void StaticMeshRenderer::OnInspector_MeshGUI()
	{
		String out;
		String in = "None";
		if (mMeshAssetHandle && mMeshAssetHandle->IsValid())
		{
			in = mMeshAssetHandle->GetAsset()->GetAssetName();
		}
		if (ImGui::AssetField("Mesh    ", in, EAssetFormat::Mesh, out))
		{
			mMeshAssetHandle = GetGameWorld()->GetAssetManager()->RequestOriginAsset<IMesh>(out);
		}
	}
	void StaticMeshRenderer::OnInspector_MaterialGUI()
	{
		String out;
		String in = "None";



		auto cnt = mMaterialAssetHandleList.size();
		bool isOpenTree = ImGui::TreeNodeEx(("Material : " + std::to_string(cnt)).c_str(), ImGuiTreeNodeFlags_DefaultOpen);
		ImGui::SameLine();
		if (ImGui::SmallButton("+"))
		{
			mMaterialAssetHandleList.push_back(nullptr);
		}ImGui::SameLine();
		if (ImGui::SmallButton("-"))
		{
			mMaterialAssetHandleList.pop_back();
		}


		if (isOpenTree)
		{
			cnt = mMaterialAssetHandleList.size();
			for (i32 i = 0; i < cnt; ++i)
			{
				String out;
				String in = "None";
				if (mMaterialAssetHandleList[i] && mMaterialAssetHandleList[i]->IsValid())
				{
					in = mMaterialAssetHandleList[i]->GetAsset()->GetAssetName();
				}

				if (ImGui::AssetField("Slot " + std::to_string(i), in, EAssetFormat::Material, out))
				{
					mMaterialAssetHandleList[i] = GetGameWorld()->GetAssetManager()->RequestOriginAsset<IMaterial>(out);
				}
				if (ImGui::TreeNodeEx("Property"))
				{
					if (mMaterialAssetHandleList[i] && mMaterialAssetHandleList[i]->IsValid())
					{
						OnInspector_MaterialPropertyGUI(mMaterialAssetHandleList[i]->GetAsset()->Get());
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
	void StaticMeshRenderer::OnInspector_MaterialPropertyGUI(SharedPtr<IMaterial> material)
	{
		//auto propertyList = material->GetPropertyList();
		//for (auto& _pair : propertyList)
		//{
		//	auto type = _pair.first;
		//	auto name = _pair.second;
		//	ImGui::Text(("Name : " + name).c_str());
		//	switch (type)
		//	{
		//	case EShaderDataType::_bool:
		//		break;
		//	case EShaderDataType::_float:
		//		break;
		//	case EShaderDataType::_float2:
		//		break;
		//	case EShaderDataType::_float3:
		//		break;
		//	case EShaderDataType::_float4:
		//		break;
		//	case EShaderDataType::_int:
		//		break;
		//	case EShaderDataType::_int2:
		//		break;
		//	case EShaderDataType::_int3:
		//		break;
		//	case EShaderDataType::_int4:
		//		break;
		//	case EShaderDataType::_uint:
		//		break;
		//	case EShaderDataType::_uint2:
		//		break;
		//	case EShaderDataType::_uint3:
		//		break;
		//	case EShaderDataType::_uint4:
		//		break;



		//	}
		//}
	}
}