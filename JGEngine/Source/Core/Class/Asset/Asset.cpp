#include "pch.h"
#include "Asset.h"
#include "AssetManager.h"
#include "Application.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"
#include "ExternalImpl/JGImGui.h"
#include "AssetImporter.h"
#include "Imgui/imgui.h"
namespace JG
{
	void TextureAssetStock::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		jsonData->AddMember("Name", Name);
		jsonData->AddMember("Width", Width);
		jsonData->AddMember("Height", Height);
		jsonData->AddMember("Channels", Channels);
		jsonData->AddMember("PixelPerUnit", PixelPerUnit);
		jsonData->AddMember("Pixels", Pixels);

	}
	void TextureAssetStock::LoadJson(SharedPtr<JsonData> jsonData)
	{
		auto Val = jsonData->GetMember("Name");
		if (Val && Val->IsString())
		{
			Name = Val->GetString();
		}
		Val = jsonData->GetMember("Width");
		if (Val && Val->IsInt32())
		{
			Width = Val->GetInt32();
		}
		Val = jsonData->GetMember("Height");
		if (Val && Val->IsInt32())
		{
			Height = Val->GetInt32();
		}
		Val = jsonData->GetMember("Channels");
		if (Val && Val->IsInt32())
		{
			Channels = Val->GetInt32();
		}
		Val = jsonData->GetMember("PixelPerUnit");
		if (Val && Val->GetUint32())
		{
			PixelPerUnit = Val->GetUint32();
		}

		Val = jsonData->GetMember("Pixels");
		if (Val && Val->IsByteList())
		{
			Pixels = Val->GetByteList();
		}
	}
	void StaticMeshAssetStock::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		jsonData->AddMember("Name", Name);
		jsonData->AddMember("IsSkinned", IsSkinned);
		jsonData->AddMember("BoundingBox", BoundingBox);

		auto meshJson = jsonData->CreateJsonData();
		auto cnt = SubMeshNames.size();
		for (auto i = 0; i < cnt; ++i)
		{
			auto viJson = meshJson->CreateJsonData();
			viJson->AddMember("Name", SubMeshNames[i]);



			auto vArrayJson = viJson->CreateJsonData();
			for (auto& v : Vertices[i])
			{
				auto vertexJson = viJson->CreateJsonData();
				v.MakeJson(vertexJson);
				vArrayJson->AddMember(vertexJson);
			}

			viJson->AddMember("Vertices", vArrayJson);
			viJson->AddMember("Indices", Indices[i]);


			meshJson->AddMember(viJson);
		}
		jsonData->AddMember("SubMeshs", meshJson);
	}
	void StaticMeshAssetStock::LoadJson(SharedPtr<JsonData> jsonData)
	{
		SubMeshNames.clear();
		Vertices.clear();
		Indices.clear();
		auto val = jsonData->GetMember("Name");
		if (val)
		{
			Name = val->GetString();
		}
		val = jsonData->GetMember("IsSkinned");
		if (val)
		{
			IsSkinned = val->GetBool();
		}
		val = jsonData->GetMember("BoundingBox");
		if (val)
		{
			BoundingBox = val->GetBoundingBox();
		}
		val = jsonData->GetMember("SubMeshs");
		if (val)
		{
			auto cnt = val->GetSize();
			Vertices.resize(cnt);
			Indices.resize(cnt);
			SubMeshNames.resize(cnt);
			for (auto i = 0; i < cnt; ++i)
			{
				auto meshJson = val->GetJsonDataFromIndex(i);

				auto verticesJson = meshJson->GetMember("Vertices");
				auto indicesJson = meshJson->GetMember("Indices");
				auto meshName = meshJson->GetMember("Name");

				SubMeshNames[i] = meshName->GetString();
				Vertices[i].resize(verticesJson->GetSize());
				Indices[i].resize(indicesJson->GetSize());
				for (auto j = 0; j < verticesJson->GetSize(); ++j)
				{
					auto vJson = verticesJson->GetJsonDataFromIndex(j);
					JGVertex v;
					v.LoadJson(vJson);
					Vertices[i][j] = v;
				}
				for (auto j = 0; j < indicesJson->GetSize(); ++j)
				{
					auto iJson = indicesJson->GetJsonDataFromIndex(j);
					u32 index;
					index = iJson->GetUint32();
					Indices[i][j] = index;
				}

			}
		}
	}
	void MaterialAssetStock::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		jsonData->AddMember("Name", Name);
		jsonData->AddMember("ShaderTemplate", ShaderTemplate);
		jsonData->AddMember("ShaderScript", ShaderScript);
		auto dataMapJson = jsonData->CreateJsonData();
		for (auto& _pair : MaterialDatas)
		{
			//
			auto name  = _pair.first;
			auto type  = _pair.second.first;
			auto value = _pair.second.second;

			//
			auto nameJson = jsonData->CreateJsonData();
			auto typeJson = jsonData->CreateJsonData();
			auto valueJson = value;

			nameJson->SetString(name);
			typeJson->SetString(ShaderDataTypeToString(type));

			auto dataJson = jsonData->CreateJsonData();
			dataJson->AddMember("Name",nameJson);
			dataJson->AddMember("Type",typeJson);
			dataJson->AddMember("Value", valueJson);
			dataMapJson->AddMember(dataJson);
		}
		jsonData->AddMember("MaterialDataMap", dataMapJson);
	}
	void MaterialAssetStock::LoadJson(SharedPtr<JsonData> jsonData)
	{
		auto val = jsonData->GetMember("Name");
		if (val && val->IsString())
		{
			Name = val->GetString();
		}

		val = jsonData->GetMember("ShaderTemplate");
		if (val && val->IsString())
		{
			ShaderTemplate = val->GetString();
		}

		val = jsonData->GetMember("ShaderScript");
		if (val && val->IsString())
		{
			ShaderScript = val->GetString();
		}

		val = jsonData->GetMember("MaterialDataMap");
		if (val && val->IsArray())
		{
			for (u64 i = 0; i < val->GetSize(); ++i)
			{
				auto dataJson = val->GetJsonDataFromIndex(i);

				auto dataVal = dataJson->GetMember("Name");
				String name;
				if (dataVal)
				{
					name = dataVal->GetString();
				}
				EShaderDataType type;
				dataVal = dataJson->GetMember("Type");
				if (dataVal)
				{
					type = StringToShaderDataType(dataVal->GetString());
				}
				dataVal = dataJson->GetMember("Value");
				if (dataVal)
				{
					MaterialDatas.emplace(name, std::pair<EShaderDataType, SharedPtr<JsonData>>(type, dataVal));
				}
			}
		}
	}

	bool MaterialAssetStock::Write(const String& path, const MaterialAssetStock& stock)
	{
		auto filePath = CombinePath(path, stock.Name) + JG_ASSET_FORMAT;

		auto json = CreateSharedPtr<Json>();
		json->AddMember(JG_ASSET_KEY, stock);

		return AssetDataBase::GetInstance().WriteAsset(filePath, EAssetFormat::Material, json);
	}

	void AssetInspectorGUI::InspectorGUI(IAsset* targetAsset)
	{
		if (targetAsset == nullptr)
		{
			return;
		}
		if (targetAsset->GetType() == JGTYPE(Asset<IMaterial>))
		{
			Material_InsepctorGUI(static_cast<Asset<IMaterial>*>(targetAsset));
		}
	}

	void AssetInspectorGUI::Material_InsepctorGUI(Asset<IMaterial>* targetAsset)
	{
		if (targetAsset->Get() == nullptr)
		{
			return;
		}
		auto propertyList = targetAsset->Get()->GetPropertyList();
		auto material = targetAsset->Get();
		Dictionary<String, std::pair<EShaderDataType, SharedPtr<JsonData>>> materialDatas;
		auto json = CreateSharedPtr<Json>();

		// Property Settings
		for (auto& property : propertyList)
		{
			
			auto type = property.first;
			auto name = property.second;
			auto dataJson = json->CreateJsonData();
			auto imguiID = "##" + name;
			ImGui::Text(name.c_str()); ImGui::SameLine();
			switch (property.first)
			{
			case EShaderDataType::_int:
			{
				i32 value = 0;
				material->GetInt(name, &value);
				if (ImGui::InputInt(imguiID.c_str(), &value) == true)
				{
					material->SetInt(name, value);
				}
				dataJson->SetInt32(value);
			}
				break;
			case EShaderDataType::_int2:
			{
				JVector2Int value = 0;
				material->GetInt2(name, &value);
				if (ImGui::InputInt2(imguiID.c_str(), (i32*)&value) == true)
				{
					material->SetInt2(name, value);
				}
				dataJson->SetVector2Int(value);
			}
				break;
			case EShaderDataType::_int3:
			{
				JVector3Int value = 0;
				material->GetInt3(name, &value);
				if (ImGui::InputInt2(imguiID.c_str(), (i32*)&value) == true)
				{
					material->SetInt3(name, value);
				}
				dataJson->SetVector3Int(value);
			}
				break;
			case EShaderDataType::_int4:
			{
				JVector4Int value = 0;
				material->GetInt4(name, &value);
				if (ImGui::InputInt4(imguiID.c_str(), (i32*)&value) == true)
				{
					material->SetInt4(name, value);
				}
				dataJson->SetVector4Int(value);
			}
				break;
			case EShaderDataType::_uint:
			{
				u32 value = 0;
				material->GetUint(name, &value);
				if (ImGui::InputInt(imguiID.c_str(), (i32*)&value) == true)
				{
					material->SetUint(name, value);
				}
				dataJson->SetUint32(value);
			}
				break;
			case EShaderDataType::_uint2:
			{
				JVector2Uint value;
				material->GetUint2(name, &value);
				if (ImGui::InputInt2(imguiID.c_str(), (i32*)&value) == true)
				{
					material->SetUint2(name, value);
				}
				dataJson->SetVector2Uint(value);
			}
				break;
			case EShaderDataType::_uint3:
			{
				JVector3Uint value;
				material->GetUint3(name, &value);
				if (ImGui::InputInt3(imguiID.c_str(), (i32*)&value) == true)
				{
					material->SetUint3(name, value);
				}
				dataJson->SetVector3Uint(value);
			}
				break;
			case EShaderDataType::_uint4:
			{
				JVector4Uint value;
				material->GetUint4(name, &value);
				if (ImGui::InputInt4(imguiID.c_str(), (i32*)&value) == true)
				{
					material->SetUint4(name, value);
				}
				dataJson->SetVector4Uint(value);
			}
				break;
			case EShaderDataType::_float:
			{
				
				f32 value;
				material->GetFloat(name, &value);
				if (ImGui::SliderFloat(imguiID.c_str(), &value, 0.0f, 1.0f) == true)
				{
					material->SetFloat(name, value);
				}
				dataJson->SetFloat(value);
			}
				break;
			case EShaderDataType::_float2:
			{
				JVector2 value;
				material->GetFloat2(name, &value);
				if (ImGui::InputFloat2(imguiID.c_str(), (f32*)&(value)) == true)
				{
					material->SetFloat2(name, value);
				}
				dataJson->SetVector2(value);
			}
				break;
			case EShaderDataType::_float3:
			{
				JVector3 value;
				material->GetFloat3(name, &value);
				if (ImGui::SliderFloat3(imguiID.c_str(), (f32*)&(value), 0.0f, 1.0f) == true)
				{
					material->SetFloat3(name, value);
				}
				dataJson->SetVector3(value);
			}
				break;
			case EShaderDataType::_float4:
			{
				JVector4 value;
				material->GetFloat4(name, &value);
				if (ImGui::InputFloat4(imguiID.c_str(), (float*)&(value)) == true)
				{
					material->SetFloat4(name, value);
				}
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
				String texturePath;
				if (ImGui::TextureAssetField(textureID, texturePath) == true)
				{
					auto asset    = AssetDataBase::GetInstance().LoadOriginAsset(texturePath);
					auto tAsset   = asset->As<ITexture>();
					if (tAsset)
					{
						material->SetTexture(name, 0, tAsset->Get());
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

			materialDatas[name] = std::pair<EShaderDataType, SharedPtr<JsonData>>(type, dataJson);
		}
		if (ImGui::Button("Save") == true)
		{
			auto fullPath     = targetAsset->GetAssetFullPath();
			auto materialJson = CreateSharedPtr<Json>();
			if (Json::Read(fullPath, materialJson) == true)
			{
				auto assetVal = materialJson->GetMember(JG_ASSET_KEY);
				if (assetVal != nullptr)
				{
					MaterialAssetStock stock;
					stock.LoadJson(assetVal);

					// Property
					stock.MaterialDatas = materialDatas;

					auto outputPath = ReplaceAll(targetAsset->GetAssetFullPath(), targetAsset->GetAssetName() + targetAsset->GetExtension(), "");
					MaterialAssetStock::Write(outputPath, stock);
					AssetDataBase::GetInstance().RefreshAsset(targetAsset->GetAssetID());
				}
			}
				
		}
	}

	void AssetInspectorGUI::Material_Save(Asset<IMaterial>* targetAsset)
	{

	}










	AssetDataBase::AssetDataBase()
	{
	
	}
	AssetDataBase::~AssetDataBase()
	{
		mAssetManagerPool.clear();

		if (mAssetLoadScheduleHandle)
		{
			mAssetLoadScheduleHandle->Reset();
			mAssetLoadScheduleHandle = nullptr;
		}
		if (mAssetUnLoadScheduleHandle)
		{
			mAssetUnLoadScheduleHandle->Reset();
			mAssetUnLoadScheduleHandle = nullptr;
		}
	


		while(mLoadAssetDataQueue.empty() == false)
		{
			mLoadAssetDataQueue.pop();
		}
		while (mUnLoadAssetDataQueue.empty() == false)
		{
			mUnLoadAssetDataQueue.pop();
		}
		mOriginAssetDataPool.clear();
		mAssetDataPool.clear();
	}
	SharedPtr<AssetManager> AssetDataBase::RequestAssetManager()
	{
		SharedPtr<AssetManager> result = nullptr;
		result = CreateSharedPtr<AssetManager>();
		mAssetManagerPool[result.get()] = result;
		return result;
	}
	void AssetDataBase::ReturnAssetManager(SharedPtr<AssetManager> assetManager)
	{
		mAssetManagerPool.erase(assetManager.get());
	}



	SharedPtr<IAsset> AssetDataBase::LoadOriginAsset(const String& path)
	{
		String resourcePath;
		String absolutePath;
		if (GetResourcePath(path, absolutePath, resourcePath) == false)
		{
			return nullptr;
		}

		if (mAssetLoadScheduleHandle == nullptr)
		{
			mAssetLoadScheduleHandle = 
				Scheduler::GetInstance().ScheduleByFrame(0, 1, -1, SchedulePriority::EndSystem, SCHEDULE_BIND_FN(&AssetDataBase::LoadAsset_Update));
			mMaxLoadAssetDataCount = (Scheduler::GetInstance().GetThreadCount() / 3) * 2;
			mAyncLoadAssetHandleList.resize(mMaxLoadAssetDataCount);
		}

		// 에셋 검사
		auto iter = mOriginAssetDataPool.find(resourcePath);
		if (iter != mOriginAssetDataPool.end())
		{
			return iter->second->Asset->Copy();
		}
	
		auto assetID   = RequestOriginAssetID(resourcePath);
		auto assetData = CreateUniquePtr<AssetData>();
		AssetLoadData assetLoadData;

		assetData->ID       = assetID;
		assetData->State    = EAssetDataState::Loading;
		assetData->Path		= resourcePath;
		assetData->Asset    = CreateAsset(assetID, absolutePath);
		auto result = assetData->Asset;


		assetLoadData.ID	= assetID;
		assetLoadData.Asset = assetData->Asset;
		strcpy(assetLoadData.Path, absolutePath.c_str());

		// 에셋 추가
		mOriginAssetDataPool.emplace(resourcePath, assetData.get());
		mLoadAssetDataQueue.push(assetLoadData);
		mAssetDataPool.emplace(assetID, std::move(assetData));
	
		return result;
	}
	SharedPtr<IAsset> AssetDataBase::LoadReadWriteAsset(AssetID originID)
	{
		auto iter = mAssetDataPool.find(originID);
		if (iter == mAssetDataPool.end())
		{
			return nullptr;
		}
		auto assetRWID = RequestRWAssetID(originID);
		auto originAssetData = mAssetDataPool[originID].get();

		String resourcePath;
		String absolutePath;
		if (GetResourcePath(originAssetData->Path, absolutePath, resourcePath) == false)
		{
			return nullptr;
		}


		auto assetData = CreateUniquePtr<AssetData>();
		assetData->ID    = assetRWID;
		assetData->State = EAssetDataState::Loading;
		assetData->Path  = resourcePath;
		assetData->Asset = CreateAsset(assetRWID, absolutePath);
		auto result = assetData->Asset;
		AssetLoadData assetLoadData;
		assetLoadData.ID    = assetRWID;
		assetLoadData.Asset = assetData->Asset;
		strcpy(assetLoadData.Path, absolutePath.c_str());



		mLoadAssetDataQueue.push(assetLoadData);
		mAssetDependencies[originID].insert(assetRWID);
		mAssetDataPool.emplace(assetRWID, std::move(assetData));

		return result;
	}
	void AssetDataBase::UnLoadAsset(AssetID id)
	{
		if (mAssetUnLoadScheduleHandle == nullptr)
		{
			mAssetUnLoadScheduleHandle =
				Scheduler::GetInstance().ScheduleByFrame(0, 1, -1, SchedulePriority::BeginSystem, SCHEDULE_BIND_FN(&AssetDataBase::UnLoadAsset_Update));
		}
		auto iter = mAssetDataPool.find(id);
		if (iter == mAssetDataPool.end())
		{
			return;
		}
		AssetUnLoadData unLoadData;
		unLoadData.Asset = iter->second->Asset;
		unLoadData.ID = iter->second->ID;
		mUnLoadAssetDataQueue.push(unLoadData);
		if (id.IsOrigin())
		{
			mOriginAssetDataPool.erase(id.ResourcePath);
		}
		else
		{
			AssetID originID;
			originID.ID = id.GetOriginID();
			originID.Origin = id.GetOriginID();
			mAssetDependencies[originID].erase(id);
		}
		mAssetDataPool.erase(id);
	}

	void AssetDataBase::RefreshAsset(AssetID originID)
	{
		if (originID.IsOrigin()== false) return;
		if (mAssetDependencies.find(originID) == mAssetDependencies.end())
		{
			return;
		}

		// Origin Update
		{
			if (mAssetDataPool.find(originID) == mAssetDataPool.end()) return;

			auto originAssetData = mAssetDataPool[originID].get();

			AssetLoadData assetLoadData;
			assetLoadData.ID = originAssetData->ID;
			assetLoadData.Asset = originAssetData->Asset;
			strcpy(assetLoadData.Path, (originAssetData->Asset->GetAssetFullPath()).c_str());
			mLoadAssetDataQueue.push(assetLoadData);

		}



		// Dependency Asset Update
		auto& dependenciesSet = mAssetDependencies[originID];
		List<AssetID> garbageAssetList;
		for (auto& assetID : dependenciesSet)
		{
			if (mAssetDataPool.find(assetID) == mAssetDataPool.end())
			{
				garbageAssetList.push_back(assetID);
			};
			auto rwAssetData = mAssetDataPool[assetID].get();
			AssetLoadData assetLoadData;
			assetLoadData.ID    = rwAssetData->ID;
			assetLoadData.Asset = rwAssetData->Asset;
			strcpy(assetLoadData.Path, (rwAssetData->Asset->GetAssetFullPath()).c_str());
			mLoadAssetDataQueue.push(assetLoadData);
		}

		for (auto& garbage : garbageAssetList)
		{
			dependenciesSet.erase(garbage);
		}
	}

	AssetID AssetDataBase::RequestOriginAssetID(const String& resourcePath)
	{
		AssetID id;
		id.Origin = mAssetIDOffset++;
		id.ID = id.Origin;
		strcpy(id.ResourcePath, resourcePath.c_str());
		return id;
	}

	AssetID AssetDataBase::RequestRWAssetID(AssetID originID)
	{
		if (originID.IsValid() == false || originID.IsOrigin() == false)
		{
			return AssetID();
		}
		if (mAssetDataPool.find(originID) == mAssetDataPool.end())
		{
			return AssetID();
		}


		AssetID id = RequestOriginAssetID(originID.ResourcePath);
		id.Origin = originID.GetID();

		mAssetDependencies[originID].insert(id);
		return id;
	}












	bool AssetDataBase::LoadAssetInternal(AssetLoadData* LoadData)
	{
		fs::path assetPath = LoadData->Path;
		if (fs::exists(assetPath) == false)
		{
			return false;
		}
		EAssetFormat assetFormat = EAssetFormat::None;
		auto json = CreateSharedPtr<Json>();
		if (ReadAsset(assetPath.string(), &assetFormat, &json) == false)
		{
			return false;
		}
		auto assetVal = json->GetMember(JG_ASSET_KEY);
		if (assetVal == nullptr)
		{
			return false;
		}

		// 에셋 로드
		switch (assetFormat)
		{
		case EAssetFormat::Texture:
		{
			LoadData->Stock = CreateSharedPtr<TextureAssetStock>();
			LoadData->OnComplete = std::bind(&AssetDataBase::TextureAsset_OnCompelete, this, std::placeholders::_1);
			LoadData->Stock->LoadJson(assetVal);
			break;
		}
		case EAssetFormat::Mesh:
		{
			StaticMeshAssetStock stock;
			stock.LoadJson(assetVal);
			auto mAsset = LoadData->Asset->As<IMesh>();
			if (mAsset != nullptr)
			{
				mAsset->mData->SetMeshStock(stock);
			}
			else return false;
			break;
		}
		case EAssetFormat::Material:
		{
			MaterialAssetStock stock;
			stock.LoadJson(assetVal);
			auto shader = ShaderLibrary::GetInstance().GetShader(stock.ShaderTemplate, { stock.ShaderScript });
			if (shader != nullptr)
			{
				auto materialAsset = LoadData->Asset->As<IMaterial>();
				if (materialAsset == nullptr)
				{
					return false;
				}
				materialAsset->mData->SetShader(shader);

				for (auto& _pair : stock.MaterialDatas)
				{
					auto name  = _pair.first;
					auto type  = _pair.second.first;
					auto value = _pair.second.second;
					switch (type)
					{
					case JG::EShaderDataType::_int:
					{
						i32 int_value = value->GetInt32();
						if (materialAsset->mData->SetInt(name, int_value) == false)
						{
							JG_CORE_WARN("Failed {0} 's Param {1} Set Int", LoadData->Path, name);
						}
					}
						break;
					case JG::EShaderDataType::_int2:
					{
						JVector2Int int2_value = value->GetVector2Int();
						if (materialAsset->mData->SetInt2(name, int2_value) == false)
						{
							JG_CORE_WARN("Failed {0} 's Param {1} Set Int2", LoadData->Path, name);
						}
					}
						break;
					case JG::EShaderDataType::_int3:
					{
						JVector3Int int3_value = value->GetVector3Int();
						if (materialAsset->mData->SetInt3(name, int3_value) == false)
						{
							JG_CORE_WARN("Failed {0} 's Param {1} Set Int3", LoadData->Path, name);
						}
					}
						break;
					case JG::EShaderDataType::_int4:
					{
						JVector4Int int4_value = value->GetVector4Int();
						if (materialAsset->mData->SetInt4(name, int4_value) == false)
						{
							JG_CORE_WARN("Failed {0} 's Param {1} Set Int4", LoadData->Path, name);
						}
					}
						break;
					case JG::EShaderDataType::_uint:
					{
						u32 uint_value = value->GetUint32();
						if (materialAsset->mData->SetUint(name, uint_value) == false)
						{
							JG_CORE_WARN("Failed {0} 's Param {1} Set Uint", LoadData->Path, name);
						}
					}
						break;
					case JG::EShaderDataType::_uint2:
					{
						JVector2Uint uint2_value = value->GetVector2Uint();
						if (materialAsset->mData->SetUint2(name, uint2_value) == false)
						{
							JG_CORE_WARN("Failed {0} 's Param {1} Set Uint2", LoadData->Path, name);
						}
					}
						break;
					case JG::EShaderDataType::_uint3:
					{
						JVector3Uint uint3_value = value->GetVector3Uint();
						if (materialAsset->mData->SetUint3(name, uint3_value) == false)
						{
							JG_CORE_WARN("Failed {0} 's Param {1} Set Uint3", LoadData->Path, name);
						}
					}
						break;
					case JG::EShaderDataType::_uint4:
					{
						JVector4Uint uint4_value = value->GetVector4Uint();
						if (materialAsset->mData->SetUint4(name, uint4_value) == false)
						{
							JG_CORE_WARN("Failed {0} 's Param {1} Set Uint4", LoadData->Path, name);
						}
					}
						break;
					case JG::EShaderDataType::_float:
					{
						f32 f32_value = value->GetFloat();
						if (materialAsset->mData->SetFloat(name, f32_value) == false)
						{
							JG_CORE_WARN("Failed {0} 's Param {1} Set Float", LoadData->Path, name);
						}
					}
						break;
					case JG::EShaderDataType::_float2:
					{
						JVector2 float2_value = value->GetVector2();
						if (materialAsset->mData->SetFloat2(name, float2_value) == false)
						{
							JG_CORE_WARN("Failed {0} 's Param {1} Set Float2", LoadData->Path, name);
						}
					}
						break;
					case JG::EShaderDataType::_float3:
					{
						JVector3 float3_value = value->GetVector3();
						if (materialAsset->mData->SetFloat3(name, float3_value) == false)
						{
							JG_CORE_WARN("Failed {0} 's Param {1} Set Float3", LoadData->Path, name);
						}
					}
						break;
					case JG::EShaderDataType::_float4:
					{
						JVector4 float4_value = value->GetVector4();
						if (materialAsset->mData->SetFloat4(name, float4_value) == false)
						{
							JG_CORE_WARN("Failed {0} 's Param {1} Set Float4", LoadData->Path, name);
						}
					}
						break;
					case JG::EShaderDataType::texture2D:
					{
						auto textureAsset = LoadOriginAsset(value->GetString());

						if (textureAsset != nullptr & textureAsset->GetType() == JGTYPE(Asset<ITexture>))
						{
							auto t = static_cast<Asset<ITexture>*>(textureAsset.get())->Get();
							if (materialAsset->mData->SetTexture(name, 0, t) == false)
							{
								JG_CORE_WARN("Failed {0} 's Param {1} Set Float4", LoadData->Path, name);
							}
						}
					}
						break;
					default:
						break;
					}
				}
			}
			break;
		}
		default:
			JG_CORE_ERROR("{0} AssetFormat is not supported in LoadAsset", (int)assetFormat);
			break;
		}
		return true;
	}


	EScheduleResult AssetDataBase::LoadAsset_Update()
	{
		LoadCompeleteData_Update();
		LoadAssetData_Update();

		return EScheduleResult::Continue;
	}
	void AssetDataBase::LoadCompeleteData_Update()
	{
		std::lock_guard<std::mutex> lock(mCompeleteMutex);
		while (mLoadCompeleteAssetDataQueue.empty() == false)
		{
			auto compeleteData = mLoadCompeleteAssetDataQueue.front(); mLoadCompeleteAssetDataQueue.pop();

			if (compeleteData.Asset == nullptr)
			{
				auto iter = mAssetDataPool.find(compeleteData.ID);
				if (iter != mAssetDataPool.end())
				{
					JG_CORE_ERROR("Asset Load Fail  : {0}", iter->second->Path);
					if (compeleteData.ID.IsOrigin())
					{
						mOriginAssetDataPool.erase(compeleteData.ID.ResourcePath);
					}


					mAssetDataPool.erase(compeleteData.ID);
				}
			}
			else
			{
				if (compeleteData.OnComplete)
				{
					compeleteData.OnComplete(&compeleteData);
				}
				auto iter = mAssetDataPool.find(compeleteData.ID);
				if (iter != mAssetDataPool.end())
				{
					iter->second->Asset = compeleteData.Asset;
					iter->second->State = EAssetDataState::None;
					JG_CORE_INFO("Asset Load Success : {0}", iter->second->Path);
				}
			}

		}
	}
	void AssetDataBase::LoadAssetData_Update()
	{
		u32 loopCnt = 0;
		while (mLoadAssetDataQueue.empty() == false)
		{
			if (mAyncLoadAssetHandleList[loopCnt] == nullptr)
			{
				auto loadData = mLoadAssetDataQueue.front(); mLoadAssetDataQueue.pop();
				JG_CORE_INFO("Asset Loading... : {0}", loadData.Path);
				mAyncLoadAssetHandleList[loopCnt] = Scheduler::GetInstance().ScheduleAsync(
					[&](void* userData)
				{
					bool result = LoadAssetInternal((AssetLoadData*)userData);
					AssetLoadData* _LoadData = (AssetLoadData*)userData;
					AssetLoadCompeleteData data;
					data.ID = _LoadData->ID;
					if (result)
					{
						data.Asset = _LoadData->Asset;
						data.Stock = _LoadData->Stock;
					}
					data.OnComplete = _LoadData->OnComplete;
					{
						std::lock_guard<std::mutex> lock(mCompeleteMutex);
						mLoadCompeleteAssetDataQueue.push(data);
					}

				}, &loadData, sizeof(AssetLoadData));
			}
			if(mAyncLoadAssetHandleList[loopCnt] && mAyncLoadAssetHandleList[loopCnt]->GetState() == EScheduleState::Compelete)
			{
				mAyncLoadAssetHandleList[loopCnt] = nullptr;
			}
		
			++loopCnt;
			if (loopCnt >= mMaxLoadAssetDataCount)
			{
				break;
			}
		}
	}
	EScheduleResult AssetDataBase::UnLoadAsset_Update()
	{
		u32 bufferCnt = Application::GetInstance().GetGraphicsAPI()->GetBufferCount();
		List<AssetUnLoadData> aliveList;
		u32 loopCnt = 0;
		while (mUnLoadAssetDataQueue.empty() == false)
		{
			auto unLoadData = mUnLoadAssetDataQueue.front(); mUnLoadAssetDataQueue.pop();
			++unLoadData.FrameCount;

			if (unLoadData.FrameCount < bufferCnt)
			{
				aliveList.push_back(unLoadData);
			}
			else
			{
				unLoadData.Asset.reset();
				unLoadData.Asset = nullptr;
			}

			++loopCnt;
			if (loopCnt <= mMaxUnLoadAssetDataCount)
			{
				break;
			}
		}
		for (auto& data : aliveList)
		{
			mUnLoadAssetDataQueue.push(data);
		}
		
		return EScheduleResult::Continue;
	}
	void AssetDataBase::TextureAsset_OnCompelete(AssetLoadCompeleteData* data)
	{
		if (data == nullptr || data->Stock == nullptr || data->Asset == nullptr)
		{
			return;
		}
		auto textureAsset = static_cast<Asset<ITexture>*>(data->Asset.get());
		auto textureStock = static_cast<TextureAssetStock*>(data->Stock.get());
		textureStock->Name = data->Asset->GetAssetPath();
		textureAsset->mData->SetTextureMemory((const byte*)textureStock->Pixels.data(), textureStock->Width, textureStock->Height, textureStock->Channels, textureStock->PixelPerUnit);
	}

	bool AssetDataBase::GetResourcePath(const String& path, String& out_absolutePath, String& out_resourcePath) const
	{
		auto originPath = path;
		auto absolutePath      = fs::absolute(path).string();
		auto absoluteAssetPath = fs::absolute(Application::GetAssetPath()).string();
		auto homePath = fs::current_path().string();

		originPath        = ReplaceAll(originPath, "\\", "/");
		absolutePath      = ReplaceAll(absolutePath, "\\", "/");
		absoluteAssetPath = ReplaceAll(absoluteAssetPath, "\\", "/");
		homePath		  = ReplaceAll(homePath, "\\", "/");


		String resourcePath;

		// 
		if (absolutePath.find(absoluteAssetPath) == String::npos )
		{
			if (path.find_first_of("Asset/") != String::npos) {

				resourcePath = ReplaceAll(absolutePath, homePath + "/", "");
				absolutePath = ReplaceAll(absolutePath, homePath + "/", "");
				absolutePath = ReplaceAll(absolutePath, "Asset/", "");
				absolutePath = CombinePath(absoluteAssetPath, absolutePath);
				out_absolutePath = absolutePath;
				out_resourcePath = resourcePath;
				return true;
			}
		}
		// in AssetPath
		else
		{
			resourcePath = ReplaceAll(absolutePath, absoluteAssetPath, "Asset");
			out_absolutePath = absolutePath;
			out_resourcePath = resourcePath;
			return true;
		}


		JG_CORE_ERROR("{0} is not exist in AssetFolder", path);
		return false;
	}

	SharedPtr<IAsset> AssetDataBase::CreateAsset(AssetID assetID, const String& path)
	{
		// Mesh, Material, Texture
		auto assetFormat = GetAssetFormat(path);

		switch (assetFormat)
		{
		case EAssetFormat::Material: return CreateSharedPtr<Asset<IMaterial>>(assetID, path);
		case EAssetFormat::Texture:  return CreateSharedPtr<Asset<ITexture>>(assetID, path);
		case EAssetFormat::Mesh:     return CreateSharedPtr<Asset<IMesh>>(assetID, path);
		}


		return nullptr;
	}



	bool AssetDataBase::WriteAsset(const String& path, EAssetFormat format, SharedPtr<Json> json)
	{
		String absolutePath;
		String resourcePath;
		if (GetResourcePath(path, absolutePath, resourcePath) == false)
		{
			return false;
		}
		String assetJsonText = Json::ToString(json);
		u64 assetJsonLen = assetJsonText.length();


		auto headerJson = CreateSharedPtr<Json>();
		headerJson->AddMember("Version", "1.0");
		headerJson->AddMember(JG_ASSET_FORMAT_KEY, (u64)format);
		String headerJsonText = Json::ToString(headerJson);
		u64 headerJsonLen = headerJsonText.length();


		std::lock_guard<std::mutex> lock(mAssetRWMutex);
		std::ofstream fout;
		fout.open(absolutePath);

		if (fout.is_open() == false)
		{
			return false;
		}
		fout.write((const char*)(&headerJsonLen), sizeof(u64));
		fout.write(headerJsonText.c_str(), headerJsonLen);
		fout.write((const char*)(&assetJsonLen), sizeof(u64));
		fout.write(assetJsonText.c_str(), assetJsonLen);


		fout.close();
		return true;
	}
	bool AssetDataBase::ReadAsset(const String& path, EAssetFormat* out_format, SharedPtr<Json>* json)
	{
		String absolutePath;
		String resourcePath;
		if (GetResourcePath(path, absolutePath, resourcePath) == false)
		{
			return false;
		}
		std::lock_guard<std::mutex> lock(mAssetRWMutex);
		std::ifstream fin;
		fin.open(absolutePath);
		if (fin.is_open() == false)
		{
			return false;
		}

		u64 assetJsonLen = 0;
		String assetJsonText;

		u64 headerJsonLen = 0;
		String headerJsonText;


		fin.read((char*)&headerJsonLen, sizeof(u64)); headerJsonText.resize(headerJsonLen);
		fin.read((char*)&headerJsonText[0], headerJsonLen);
		fin.read((char*)&assetJsonLen, sizeof(u64)); assetJsonText.resize(assetJsonLen);
		fin.read((char*)&assetJsonText[0], assetJsonLen);




		auto headerJson = Json::ToObject(headerJsonText);
		auto assetJson = Json::ToObject(assetJsonText);


		auto val = headerJson->GetMember(JG_ASSET_FORMAT_KEY);



		if (val && val->IsUint64())
		{
			*out_format = (EAssetFormat)val->GetUint64();
		}
		else
		{
			return false;
		}

		*json = assetJson;
		fin.close();

		return true;
	}

	EAssetFormat AssetDataBase::GetAssetFormat(const String& path)
	{
		String absolutePath;
		String resourcePath;
		if (GetResourcePath(path, absolutePath, resourcePath) == false)
		{
			return EAssetFormat::None;
		}
		EAssetFormat assetFormat = EAssetFormat::None;

		std::ifstream fin;
		fin.open(absolutePath);
		if (fin.is_open() == true)
		{
			u64 headerJsonLen = 0;
			String headerJsonText;
			fin.read((char*)&headerJsonLen, sizeof(u64)); headerJsonText.resize(headerJsonLen);
			fin.read((char*)&headerJsonText[0], headerJsonLen);

			auto headerJson = Json::ToObject(headerJsonText);
			auto val = headerJson->GetMember(JG_ASSET_FORMAT_KEY);
			if (val && val->IsUint64())
			{
				assetFormat = (EAssetFormat)val->GetUint64();
			}
		}
		return assetFormat;
	}

}