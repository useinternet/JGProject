#include "pch.h"
#include "Asset.h"
#include "AssetManager.h"
#include "Application.h"
#include "Graphics/JGGraphics.h"
#include "AssetImporter.h"

namespace JG
{
	void TextureAssetStock::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		jsonData->AddMember("Width", Width);
		jsonData->AddMember("Height", Height);
		jsonData->AddMember("Channels", Channels);
		jsonData->AddMember("PixelPerUnit", PixelPerUnit);
		jsonData->AddMember("OriginPixelSize", OriginPixelSize);
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
		Val = jsonData->GetMember("OriginPixelSize");
		if (Val && Val->GetUint32())
		{
			OriginPixelSize = Val->GetUint32();
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
		auto filePath = PathExtend::CombinePath(path, stock.Name) + JG_ASSET_FORMAT;

		auto json = CreateSharedPtr<Json>();
		json->AddMember(JG_ASSET_KEY, stock);

		return AssetDataBase::GetInstance().WriteAsset(filePath, EAssetFormat::Material, json);
	}
	const String& IAsset::GetAssetRootPath() const
	{
		return Application::GetAssetPath();
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



	AssetID AssetDataBase::GetAssetOriginID(const String& path)
	{
		String resourcePath;
		String absolutePath;
		if (GetResourcePath(path, absolutePath, resourcePath) == false)
		{
			return AssetID();
		}
		if (mOriginAssetDataPool.find(resourcePath) == mOriginAssetDataPool.end())
		{
			return AssetID();
		}

		return mOriginAssetDataPool[resourcePath]->ID;
	}

	SharedPtr<IAsset> AssetDataBase::LoadOriginAsset(const String& path)
	{
		String resourcePath;
		String absolutePath;
		if (GetResourcePath(path, absolutePath, resourcePath) == false)
		{
			return nullptr;
		}
		if (fs::exists(absolutePath) == false)
		{
			return nullptr;
		}
		if (mAssetLoadScheduleHandle == nullptr)
		{
			mAssetLoadScheduleHandle = 
				Scheduler::GetInstance().ScheduleByFrame(0, 1, -1, SchedulePriority::AssetDataBase_Update, SCHEDULE_BIND_FN(&AssetDataBase::LoadAsset_Update));
			mMaxLoadAssetDataCount = (Scheduler::GetInstance().GetThreadCount() / 3) * 2;
			mAyncLoadAssetHandleList.resize(mMaxLoadAssetDataCount);
		}

		// 에셋 검사

		std::lock_guard<std::mutex> lock(mAssetLoadMutex);
		auto iter = mOriginAssetDataPool.find(resourcePath);
		if (iter != mOriginAssetDataPool.end())
		{
			if (iter->second->Asset == nullptr)
			{
				return nullptr;
			}
			else
			{
				return iter->second->Asset->Copy();
			}
		
		}
	
		auto assetID   = RequestOriginAssetID();
		auto assetData = CreateUniquePtr<AssetData>();
		SharedPtr<AssetLoadData> assetLoadData = CreateSharedPtr<AssetLoadData>();


		assetData->State    = EAssetDataState::Loading;
		assetData->Path		= resourcePath;
		assetData->Asset    = CreateAsset(assetID, absolutePath);
		assetData->ID = assetData->Asset->GetAssetID();
		auto result = assetData->Asset;


		assetLoadData->ID	 = assetData->ID;
		assetLoadData->Asset = assetData->Asset;
		strcpy(assetLoadData->Path, absolutePath.c_str());

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

		auto originAssetData = mAssetDataPool[originID].get();

		String resourcePath;
		String absolutePath;
		if (GetResourcePath(originAssetData->Path, absolutePath, resourcePath) == false)
		{
			return nullptr;
		}

		std::lock_guard<std::mutex> lock(mAssetLoadMutex);
		auto assetRWID = RequestRWAssetID(originID);
		auto assetData = CreateUniquePtr<AssetData>();

		assetData->State = EAssetDataState::Loading;
		assetData->Path  = resourcePath;
		assetData->Asset = CreateAsset(assetRWID, absolutePath);
		assetData->ID = assetData->Asset->GetAssetID();
		auto result = assetData->Asset;
		SharedPtr<AssetLoadData> assetLoadData = CreateSharedPtr<AssetLoadData>();
		assetLoadData->ID    = assetData->ID;
		assetLoadData->Asset = assetData->Asset;
		strcpy(assetLoadData->Path, absolutePath.c_str());



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



	void AssetDataBase::RefreshAsset(AssetID originID, const String& reName)
	{
		if (originID.IsOrigin()== false) return;
		if (mAssetDependencies.find(originID) == mAssetDependencies.end())
		{
			return;
		}
		if (reName.length() > 0)
		{
			RefreshAssetName(originID, reName);
		}
		// Origin Update
		{
			if (mAssetDataPool.find(originID) == mAssetDataPool.end()) return;

			auto originAssetData = mAssetDataPool[originID].get();
			SharedPtr<AssetLoadData> assetLoadData = CreateSharedPtr<AssetLoadData>();
			assetLoadData->ID    = originAssetData->ID;
			assetLoadData->Asset = originAssetData->Asset;
			strcpy(assetLoadData->Path, (originAssetData->Asset->GetAssetFullPath()).c_str());
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
			SharedPtr<AssetLoadData> assetLoadData = CreateSharedPtr<AssetLoadData>();
			assetLoadData->ID    = rwAssetData->ID;
			assetLoadData->Asset = rwAssetData->Asset;
			strcpy(assetLoadData->Path, (rwAssetData->Asset->GetAssetFullPath()).c_str());
			mLoadAssetDataQueue.push(assetLoadData);
		}

		for (auto& garbage : garbageAssetList)
		{
			dependenciesSet.erase(garbage);
		}
	}
	void AssetDataBase::RefreshAssetName(AssetID originID, const String& reName)
	{
	
		if (originID.IsOrigin() == false) return;
		if (mAssetDataPool.find(originID) == mAssetDataPool.end()) return;

		 

		auto originData = mAssetDataPool[originID].get();
		EAssetFormat assetFormat = originData->Asset->GetAssetFormat();
		String absolutePath;
		String resourcePath;
		if (GetResourcePath(reName, absolutePath, resourcePath) == false)
		{
			return;
		}
		auto oldResourcePath = originData->Asset->GetAssetPath();
		originData->Asset->Set(originData->ID, reName, assetFormat);
		originData->ID = originData->Asset->GetAssetID();
		originData->Path = originData->Asset->GetAssetPath();
		


		mOriginAssetDataPool.erase(oldResourcePath);
		mOriginAssetDataPool.emplace(originData->Path, originData);

		if (mAssetDependencies.find(originID) == mAssetDependencies.end())
		{
			return;
		}

		auto& dependenciesSet = mAssetDependencies[originData->ID];
		List<AssetID> garbageAssetList;
		for (auto& assetID : dependenciesSet)
		{
			if (mAssetDataPool.find(assetID) == mAssetDataPool.end())
			{
				garbageAssetList.push_back(assetID);
			};

			auto rwAssetData = mAssetDataPool[assetID].get();
			rwAssetData->Asset->Set(rwAssetData->ID, reName, assetFormat);
			rwAssetData->ID   = rwAssetData->Asset->GetAssetID();
			rwAssetData->Path = rwAssetData->Asset->GetAssetPath();
		}
	}
	AssetID AssetDataBase::RequestOriginAssetID()
	{
		AssetID id;
		id.Origin = mAssetIDOffset++;
		id.ID = id.Origin;
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


		AssetID id = RequestOriginAssetID();
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
		LoadData->Json = CreateSharedPtr<Json>();

		if (ReadAsset(assetPath.string(), &assetFormat, &LoadData->Json) == false)
		{
			return false;
		}
		auto assetVal = LoadData->Json->GetMember(JG_ASSET_KEY);
		if (assetVal == nullptr)
		{
			return false;
		}

		// 에셋 로드
		switch (assetFormat)
		{
		case EAssetFormat::CubeMap:
		case EAssetFormat::Texture:
		{
			auto stock = CreateSharedPtr<TextureAssetStock>();
			LoadData->Stock = stock;
			LoadData->OnComplete = std::bind(&AssetDataBase::TextureAsset_OnCompelete, this, std::placeholders::_1);
			LoadData->Stock->LoadJson(assetVal);
			if (assetFormat == EAssetFormat::CubeMap)
			{
				int n = 0;
			}
			List<jbyte> uncom;
			u64 originSize = stock->OriginPixelSize + 10;
			uncom.resize(originSize);
			if (uncompress((Bytef*)uncom.data(), (uLongf*)(&originSize), (const Bytef*)stock->Pixels.data(), stock->Pixels.size()) != Z_OK)
			{
			}
			else
			{
				stock->Pixels = uncom;
				stock->Pixels.resize(originSize);
			}
			break;
		}
		case EAssetFormat::Mesh:
		{
			StaticMeshAssetStock stock;
			stock.LoadJson(assetVal);
			auto mAsset = LoadData->Asset->As<Asset<IMesh>>();
			if (mAsset != nullptr)
			{
				mAsset->mData->SetMeshStock(stock);
			}
			else return false;
			break;
		}
		case EAssetFormat::Material:
		{
			LoadData->Stock      = CreateSharedPtr<MaterialAssetStock>();
			LoadData->OnComplete = std::bind(&AssetDataBase::MaterialAsset_OnCompelete, this, std::placeholders::_1);
			LoadData->Stock->LoadJson(assetVal);
			break;
		}
		case EAssetFormat::GameWorld:
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
				JG_CORE_INFO("Asset Loading... : {0}", loadData->Path);
				mAyncLoadAssetHandleList[loopCnt] = Scheduler::GetInstance().ScheduleAsync(
					[&](SharedPtr<IJGObject> userData)
				{
					bool result = LoadAssetInternal(userData->As<AssetLoadData>());
					AssetLoadData* _LoadData = userData->As<AssetLoadData>();
					AssetLoadCompeleteData data;
					data.ID = _LoadData->ID;
					if (result)
					{
						data.Asset = _LoadData->Asset;
						data.Stock = _LoadData->Stock;
						data.Json  = _LoadData->Json;
					}
					data.OnComplete = _LoadData->OnComplete;
					{
						std::lock_guard<std::mutex> lock(mCompeleteMutex);
						mLoadCompeleteAssetDataQueue.push(data);
					}

				}, loadData);
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
		u32 bufferCnt = JGGraphics::GetInstance().GetBufferCount();
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
				if (unLoadData.Asset->GetAssetID().IsOrigin())
				{
					JG_CORE_INFO("UnLoad Asset : {0}", unLoadData.Asset->GetAssetPath());

				}
				else
				{
					JG_CORE_INFO("UnLoad ReadWrite Asset : {0}", unLoadData.Asset->GetAssetPath());
				}
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

		switch (textureAsset->GetAssetFormat())
		{
		case EAssetFormat::Texture:
			textureAsset->mData->SetTextureMemory((const byte*)textureStock->Pixels.data(), textureStock->Width, textureStock->Height, textureStock->Channels, textureStock->PixelPerUnit);
			break;
		case EAssetFormat::CubeMap:
			textureAsset->mData->SetTextureMemory(
				(const byte*)textureStock->Pixels.data(), textureStock->Width, textureStock->Height, textureStock->Channels, textureStock->PixelPerUnit, 
				6, 1, ETextureFlags::SRV_TextureCube);
			break;
		}

		
	}
	void AssetDataBase::MaterialAsset_OnCompelete(AssetLoadCompeleteData* data)
	{
		if (data == nullptr || data->Stock == nullptr || data->Asset == nullptr)
		{
			return;
		}
		auto materialAsset = static_cast<Asset<IMaterial>*>(data->Asset.get());
		auto materialStock = static_cast<MaterialAssetStock*>(data->Stock.get());
		materialStock->Name = data->Asset->GetAssetPath();

		auto shader = ShaderLibrary::GetInstance().FindGraphicsShader(materialStock->ShaderTemplate, { materialStock->ShaderScript });
		if (shader != nullptr)
		{
			materialAsset->mData->SetShader(shader);

			for (auto& _pair : materialStock->MaterialDatas)
			{
				auto name = _pair.first;
				auto type = _pair.second.first;
				auto value = _pair.second.second;
				switch (type)
				{
				case JG::EShaderDataType::_int:
				{
					i32 int_value = value->GetInt32();
					if (materialAsset->mData->SetInt(name, int_value) == false)
					{
						JG_CORE_WARN("Failed {0} 's Param {1} Set Int", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_int2:
				{
					JVector2Int int2_value = value->GetVector2Int();
					if (materialAsset->mData->SetInt2(name, int2_value) == false)
					{
						JG_CORE_WARN("Failed {0} 's Param {1} Set Int2", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_int3:
				{
					JVector3Int int3_value = value->GetVector3Int();
					if (materialAsset->mData->SetInt3(name, int3_value) == false)
					{
						JG_CORE_WARN("Failed {0} 's Param {1} Set Int3", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_int4:
				{
					JVector4Int int4_value = value->GetVector4Int();
					if (materialAsset->mData->SetInt4(name, int4_value) == false)
					{
						JG_CORE_WARN("Failed {0} 's Param {1} Set Int4", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_uint:
				{
					u32 uint_value = value->GetUint32();
					if (materialAsset->mData->SetUint(name, uint_value) == false)
					{
						JG_CORE_WARN("Failed {0} 's Param {1} Set Uint", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_uint2:
				{
					JVector2Uint uint2_value = value->GetVector2Uint();
					if (materialAsset->mData->SetUint2(name, uint2_value) == false)
					{
						JG_CORE_WARN("Failed {0} 's Param {1} Set Uint2", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_uint3:
				{
					JVector3Uint uint3_value = value->GetVector3Uint();
					if (materialAsset->mData->SetUint3(name, uint3_value) == false)
					{
						JG_CORE_WARN("Failed {0} 's Param {1} Set Uint3", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_uint4:
				{
					JVector4Uint uint4_value = value->GetVector4Uint();
					if (materialAsset->mData->SetUint4(name, uint4_value) == false)
					{
						JG_CORE_WARN("Failed {0} 's Param {1} Set Uint4", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_float:
				{
					f32 f32_value = value->GetFloat();
					if (materialAsset->mData->SetFloat(name, f32_value) == false)
					{
						JG_CORE_WARN("Failed {0} 's Param {1} Set Float", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_float2:
				{
					JVector2 float2_value = value->GetVector2();
					if (materialAsset->mData->SetFloat2(name, float2_value) == false)
					{
						JG_CORE_WARN("Failed {0} 's Param {1} Set Float2", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_float3:
				{
					JVector3 float3_value = value->GetVector3();
					if (materialAsset->mData->SetFloat3(name, float3_value) == false)
					{
						JG_CORE_WARN("Failed {0} 's Param {1} Set Float3", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_float4:
				{
					JVector4 float4_value = value->GetVector4();
					if (materialAsset->mData->SetFloat4(name, float4_value) == false)
					{
						JG_CORE_WARN("Failed {0} 's Param {1} Set Float4", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::texture2D:
				{
					if (value->IsString() == true)
					{
						auto textureAsset = LoadOriginAsset(value->GetString());

						if (textureAsset != nullptr && textureAsset->GetType() == JGTYPE(Asset<ITexture>))
						{
							auto t = static_cast<Asset<ITexture>*>(textureAsset.get())->Get();
							if (materialAsset->mData->SetTexture(name, t) == false)
							{
								JG_CORE_WARN("Failed {0} 's Param {1} Set Float4", materialStock->Name, name);
							}
						}
					}
				}
				break;
				default:
					break;
				}
			}
		}
	}

	bool AssetDataBase::GetResourcePath(const String& path, String& out_absolutePath, String& out_resourcePath) const
	{
		auto originPath = path;
		auto absolutePath      = fs::absolute(path).string();
		auto absoluteAssetPath = fs::absolute(Application::GetAssetPath()).string();
		auto homePath = fs::current_path().string();

		originPath        = StringExtend::ReplaceAll(originPath, "\\", "/");
		absolutePath      = StringExtend::ReplaceAll(absolutePath, "\\", "/");
		absoluteAssetPath = StringExtend::ReplaceAll(absoluteAssetPath, "\\", "/");
		homePath		  = StringExtend::ReplaceAll(homePath, "\\", "/");


		String resourcePath;

		// 
		if (absolutePath.find(absoluteAssetPath) == String::npos )
		{
			if (path.find_first_of("Asset/") != String::npos) {

				resourcePath = StringExtend::ReplaceAll(absolutePath, homePath + "/", "");
				absolutePath = StringExtend::ReplaceAll(absolutePath, homePath + "/", "");
				absolutePath = StringExtend::ReplaceAll(absolutePath, "Asset/", "");
				absolutePath = PathExtend::CombinePath(absoluteAssetPath, absolutePath);
				out_absolutePath = absolutePath;
				out_resourcePath = resourcePath;
				return true;
			}
		}
		// in AssetPath
		else
		{
			resourcePath = StringExtend::ReplaceAll(absolutePath, absoluteAssetPath, "Asset");
			out_absolutePath = absolutePath;
			out_resourcePath = resourcePath;
			return true;
		}
		return false;
	}

	SharedPtr<IAsset> AssetDataBase::CreateAsset(AssetID assetID, const String& path)
	{
		// Mesh, Material, Texture
		auto assetFormat = GetAssetFormat(path);

		switch (assetFormat)
		{
		case EAssetFormat::Material: return CreateSharedPtr<Asset<IMaterial>>(assetID, path, assetFormat);
		case EAssetFormat::CubeMap:
		case EAssetFormat::Texture:  return CreateSharedPtr<Asset<ITexture>>(assetID, path, assetFormat);
		case EAssetFormat::Mesh:     return CreateSharedPtr<Asset<IMesh>>(assetID, path, assetFormat);
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
			if (out_format)
			{
				*out_format = (EAssetFormat)val->GetUint64();
			}
	
		}
		else
		{
			return false;
		}

		if (json)
		{
			*json = assetJson;
		}

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