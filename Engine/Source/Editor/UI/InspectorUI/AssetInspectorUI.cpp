#include "pch.h"
#include "AssetInspectorUI.h"

#include "Graphics/Material.h"
#include "Graphics/Shader.h"

#include "ExternalImpl/JGImGui.h"
namespace JG
{
	void MaterialAssetInspectorUI::OnGUI_Impl(Asset<IMaterial>* obj)
	{
		if (obj->Get() == nullptr)
		{
			return;
		}
		// Material 정보를 가져온다.
		auto propertyList = obj->Get()->GetPropertyList();


		auto material = obj->Get();
		Dictionary<String, std::pair<EShaderDataType, SharedPtr<JsonData>>> materialDatas;
		auto json = CreateSharedPtr<Json>();
		f32 label_space = 0.0f;


		ImGui::Text(obj->GetAssetName().c_str()); ImGui::Spacing();
		for (auto& property : propertyList)
		{
			auto name = property.second;
			label_space = std::max<f32>(label_space, ImGui::CalcTextSize(name.c_str()).x);
		}
		// Property Settings
		for (auto& property : propertyList)
		{

			auto type = property.first;
			auto name = property.second;
			auto dataJson = json->CreateJsonData();
			auto imguiID = "##" + name;
			switch (property.first)
			{
			case EShaderDataType::_int:
			{
				i32 value = 0;
				material->GetInt(name, &value);
				ImGui::Int_OnGUI(name, value, label_space);
				material->SetInt(name, value);
				dataJson->SetInt32(value);
			}
			break;
			case EShaderDataType::_int2:
			{
				JVector2Int value = 0;
				material->GetInt2(name, &value);
				ImGui::Vector2Int_OnGUI(name, value, label_space);
				material->SetInt2(name, value);
				dataJson->SetVector2Int(value);
			}
			break;
			case EShaderDataType::_int3:
			{
				JVector3Int value = 0;
				material->GetInt3(name, &value);
				ImGui::Vector3Int_OnGUI(name, value, label_space);
				material->SetInt3(name, value);
				dataJson->SetVector3Int(value);
			}
			break;
			case EShaderDataType::_int4:
			{
				JVector4Int value = 0;
				material->GetInt4(name, &value);
				ImGui::Vector4Int_OnGUI(name, value, label_space);
				material->SetInt4(name, value);
				dataJson->SetVector4Int(value);
			}
			break;
			case EShaderDataType::_uint:
			{
				u32 value = 0;
				material->GetUint(name, &value);
				ImGui::Uint_OnGUI(name, value, label_space);
				material->SetUint(name, value);
				dataJson->SetUint32(value);
			}
			break;
			case EShaderDataType::_uint2:
			{
				JVector2Uint value;
				material->GetUint2(name, &value);
				ImGui::Vector2Uint_OnGUI(name, value, label_space);
				material->SetUint2(name, value);
				dataJson->SetVector2Uint(value);
			}
			break;
			case EShaderDataType::_uint3:
			{
				JVector3Uint value;
				material->GetUint3(name, &value);
				ImGui::Vector3Uint_OnGUI(name, value, label_space);
				material->SetUint3(name, value);
				dataJson->SetVector3Uint(value);
			}
			break;
			case EShaderDataType::_uint4:
			{
				JVector4Uint value;
				material->GetUint4(name, &value);
				ImGui::Vector4Uint_OnGUI(name, value, label_space);
				material->SetUint4(name, value);
				dataJson->SetVector4Uint(value);
			}
			break;
			case EShaderDataType::_float:
			{

				f32 value;
				material->GetFloat(name, &value);
				ImGui::Float_OnGUI(name, value, label_space);
				material->SetFloat(name, value);
				dataJson->SetFloat(value);
			}
			break;
			case EShaderDataType::_float2:
			{
				JVector2 value;
				material->GetFloat2(name, &value);
				ImGui::Vector2_OnGUI(name, value, label_space);
				material->SetFloat2(name, value);
				dataJson->SetVector2(value);
			}
			break;
			case EShaderDataType::_float3:
			{
				JVector3 value;
				material->GetFloat3(name, &value);
				ImGui::Vector3_OnGUI(name, value, label_space);
				material->SetFloat3(name, value);
				dataJson->SetVector3(value);
			}
			break;
			case EShaderDataType::_float4:
			{
				JVector4 value;
				material->GetFloat4(name, &value);
				ImGui::Vector4_OnGUI(name, value, label_space);
				material->SetFloat4(name, value);
				dataJson->SetVector4(value);
			}
			break;
			case EShaderDataType::texture2D:
			{
				SharedPtr<ITexture> texture = nullptr;
				material->GetTexture(name, 0, &texture);


				u64 textureID = 0;
				if (texture != nullptr && texture->IsValid())
				{
					textureID = texture->GetTextureID();
					textureID = JGImGui::GetInstance().ConvertImGuiTextureID(textureID);
				}

				auto asset = ImGui::Texture_OnGUI(name, textureID, label_space);


				if (asset && asset->Is<Asset<ITexture>>())
				{
					auto tasset = asset->As<Asset<ITexture>>();
					if (tasset->IsValid())
					{
						material->SetTexture(name, 0, tasset->Get());
						texture = tasset->Get();
					}
				}

				if (texture != nullptr)
				{
					dataJson->SetString(texture->GetName());
				}
			}
			break;
			default:
				break;
			}
			ImGui::Spacing();
			materialDatas[name] = std::pair<EShaderDataType, SharedPtr<JsonData>>(type, dataJson);
		}
		if (ImGui::Button("Save") == true)
		{
			auto fullPath = obj->GetAssetFullPath();
			auto materialJson = CreateSharedPtr<Json>();
			if (AssetDataBase::GetInstance().ReadAsset(fullPath, nullptr, &materialJson) == true)
			{
				auto assetVal = materialJson->GetMember(JG_ASSET_KEY);
				if (assetVal != nullptr)
				{
					MaterialAssetStock stock;
					stock.LoadJson(assetVal);
					stock.Name = obj->GetAssetName();
					// Property
					stock.MaterialDatas = materialDatas;

					auto outputPath = StringExtend::ReplaceAll(obj->GetAssetFullPath(), obj->GetAssetName() + obj->GetExtension(), "");
					MaterialAssetStock::Write(outputPath, stock);
					AssetDataBase::GetInstance().RefreshAsset(obj->GetAssetID());
				}
			}

		}


	}
}