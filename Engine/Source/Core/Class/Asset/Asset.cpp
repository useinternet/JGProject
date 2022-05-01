#include "pch.h"
#include "Asset.h"
#include "AssetManager.h"
#include "AssetHelper.h"
#include "Application.h"
#include "Graphics/JGGraphics.h"
#include "Class/Data/Skeletone.h"
#include "Animation/AnimationClip.h"
#include "Animation/AnimationController.h"
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
		jsonData->AddMember(NAME_KEY, Name);
		jsonData->AddMember(BOUNDING_BOX_KEY, BoundingBox);

		SharedPtr<JsonData> meshJson = jsonData->CreateJsonData();
		u64 cnt = SubMeshNames.size();
		for (auto i = 0; i < cnt; ++i)
		{
			auto viJson = meshJson->CreateJsonData();
			viJson->AddMember(NAME_KEY, SubMeshNames[i]);

			SharedPtr<JsonData> vArrayJson = viJson->CreateJsonData();
			for (auto& v : Vertices[i])
			{
				SharedPtr<JsonData> vertexJson = viJson->CreateJsonData();
				v.MakeJson(vertexJson);
				vArrayJson->AddMember(vertexJson);
			}
			SharedPtr<JsonData> bvArrayJson = viJson->CreateJsonData();
			for (auto& bv : BoneVertices[i])
			{
				SharedPtr<JsonData> boneVertexJson = viJson->CreateJsonData();
				bv.MakeJson(boneVertexJson);
				bvArrayJson->AddMember(boneVertexJson);
			}


			viJson->AddMember(VERTICES_KEY, vArrayJson);
			viJson->AddMember(BONE_VERTICES_KEY, bvArrayJson);
			viJson->AddMember(INDICES_KEY, Indices[i]);


			meshJson->AddMember(viJson);
		}
		jsonData->AddMember(SUBMESHS_KEY, meshJson);
	}
	void StaticMeshAssetStock::LoadJson(SharedPtr<JsonData> jsonData)
	{
		SubMeshNames.clear();
		Vertices.clear();
		Indices.clear();
		SharedPtr<JsonData> val = jsonData->GetMember(NAME_KEY);
		if (val)
		{
			Name = val->GetString();
		}
		val = jsonData->GetMember(BOUNDING_BOX_KEY);
		if (val)
		{
			BoundingBox = val->GetBoundingBox();
		}
		val = jsonData->GetMember(SUBMESHS_KEY);
		if (val)
		{
			u64 cnt = val->GetSize();
			Vertices.resize(cnt);
			BoneVertices.resize(cnt);
			Indices.resize(cnt);
			
			SubMeshNames.resize(cnt);
			for (u64 i = 0; i < cnt; ++i)
			{
				SharedPtr<JsonData> meshJson = val->GetJsonDataFromIndex(i);

				SharedPtr<JsonData> verticesJson	 = meshJson->GetMember(VERTICES_KEY);
				SharedPtr<JsonData> boneVerticesJson = meshJson->GetMember(BONE_VERTICES_KEY);
				SharedPtr<JsonData> indicesJson		 = meshJson->GetMember(INDICES_KEY);
				SharedPtr<JsonData> meshNameJson	 = meshJson->GetMember(NAME_KEY);

				if (meshNameJson != nullptr)
				{
					SubMeshNames[i] = meshNameJson->GetString();
				}
				
				if (verticesJson != nullptr)
				{
					Vertices[i].resize(verticesJson->GetSize());
					for (u64 j = 0; j < verticesJson->GetSize(); ++j)
					{
						SharedPtr<JsonData> vJson = verticesJson->GetJsonDataFromIndex(j);
						JGVertex v;
						v.LoadJson(vJson);
						Vertices[i][j] = v;
					}
				}
				if (boneVerticesJson != nullptr)
				{
					BoneVertices[i].resize(boneVerticesJson->GetSize());
					for (u64 j = 0; j < boneVerticesJson->GetSize(); ++j)
					{
						SharedPtr<JsonData> vJson = boneVerticesJson->GetJsonDataFromIndex(j);
						JGBoneVertex v;
						v.LoadJson(vJson);
						BoneVertices[i][j] = v;
					}
				}
				if (indicesJson != nullptr)
				{
					Indices[i].resize(indicesJson->GetSize());
					for (u64 j = 0; j < indicesJson->GetSize(); ++j)
					{
						auto iJson = indicesJson->GetJsonDataFromIndex(j);
						u32 index;
						index = iJson->GetUint32();
						Indices[i][j] = index;
					}
				}
			}
		}
	}
	void SkeletalAssetStock::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		jsonData->AddMember(NAME_KEY, Name);
		jsonData->AddMember(ROOT_BONE_NODE_KEY, RootBoneNode);
		jsonData->AddMember(ROOT_OFFSET_KEY, RootOffset);

		SharedPtr<JsonData> boneNodeArrayJson = jsonData->CreateJsonData();
		for (u64 i = 0; i < BoneNodes.size(); ++i)
		{
			SharedPtr<JsonData> boneNodeJson = jsonData->CreateJsonData();

			const BoneNode& boneNode = BoneNodes[i];

			boneNodeJson->AddMember(BONE_NODE_ID_KEY, boneNode.ID);
			boneNodeJson->AddMember(BONE_NODE_NAME_KEY, boneNode.Name);
			boneNodeJson->AddMember(PARENT_BONDE_NODE_KEY, boneNode.ParentNode);
			boneNodeJson->AddMember(CHILD_BONE_NODES_KEY, boneNode.ChildNodes);
			boneNodeJson->AddMember(TRANSFORM_KEY, boneNode.Transform);
			boneNodeJson->AddMember(BONE_OFFSET_KEY, boneNode.BoneOffset);


			boneNodeArrayJson->AddMember(boneNodeJson);
		}
		jsonData->AddMember(BONE_NODES_KEY, boneNodeArrayJson);
	}
	void SkeletalAssetStock::LoadJson(SharedPtr<JsonData> jsonData)
	{
		SharedPtr<JsonData> val = jsonData->GetMember(NAME_KEY);
		if (val != nullptr)
		{
			Name = val->GetString();
		}

		val = jsonData->GetMember(ROOT_BONE_NODE_KEY);
		if (val != nullptr)
		{
			RootBoneNode = val->GetUint32();
		}

		val = jsonData->GetMember(ROOT_OFFSET_KEY);
		if (val != nullptr)
		{
			RootOffset = val->GetMatrix();
		}

		SharedPtr<JsonData> boneNodeArrayJson = jsonData->GetMember(BONE_NODES_KEY);
		if (boneNodeArrayJson != nullptr)
		{
			BoneNodes.resize(boneNodeArrayJson->GetSize());

			for (u32 i = 0; i < boneNodeArrayJson->GetSize(); ++i)
			{
				SharedPtr<JsonData> boneNodeJson = boneNodeArrayJson->GetJsonDataFromIndex(i);

				val = boneNodeJson->GetMember(BONE_NODE_ID_KEY);
				if (val != nullptr)
				{
					BoneNodes[i].ID = val->GetUint32();
				}

				val = boneNodeJson->GetMember(BONE_NODE_NAME_KEY);
				if (val != nullptr)
				{
					BoneNodes[i].Name = val->GetString();
				}

				val = boneNodeJson->GetMember(PARENT_BONDE_NODE_KEY);
				if (val != nullptr)
				{
					BoneNodes[i].ParentNode = val->GetUint32();
				}

				val = boneNodeJson->GetMember(CHILD_BONE_NODES_KEY);
				if (val != nullptr)
				{
					for (u32 j = 0; j < val->GetSize(); ++j)
					{
						BoneNodes[i].ChildNodes.push_back(val->GetJsonDataFromIndex(j)->GetUint32());
					}
				}

				val = boneNodeJson->GetMember(TRANSFORM_KEY);
				if (val != nullptr)
				{
					BoneNodes[i].Transform = val->GetMatrix();
				}

				val = boneNodeJson->GetMember(BONE_OFFSET_KEY);
				if (val != nullptr)
				{
					BoneNodes[i].BoneOffset = val->GetMatrix();
				}
			}
		}
	}
	void AnimationClipAssetStock::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		jsonData->AddMember(NAME_KEY, Name);
		jsonData->AddMember(DURATION_KEY, Duration);
		jsonData->AddMember(TICKS_PER_SECOND_KEY, TicksPerSecond);



		SharedPtr<JsonData> animNodesJson = jsonData->CreateJsonData();

		for (const auto& _pair : AnimationNodes)
		{
			SharedPtr<JsonData> animNodeJson = animNodesJson->CreateJsonData();

			animNodeJson->AddMember(NODE_NAME_KEY, _pair.first);
			animNodeJson->AddMember(LOCATION_VALUES_KEY, _pair.second.LocationValues);
			animNodeJson->AddMember(ROTATION_VALUES_KEY, _pair.second.RotationValues);
			animNodeJson->AddMember(SCALE_VALUES_KEY, _pair.second.ScaleValues);

			animNodeJson->AddMember(LOCATION_TIMES_KEY, _pair.second.LocationTimes);
			animNodeJson->AddMember(ROTATION_TIMES_KEY, _pair.second.RotationTimes);
			animNodeJson->AddMember(SCALE_TIMES_KEY, _pair.second.ScaleTimes);

			animNodesJson->AddMember(animNodeJson);
		}
		jsonData->AddMember(ANIMATION_NODES_KEY, animNodesJson);
	}

	void AnimationClipAssetStock::LoadJson(SharedPtr<JsonData> jsonData)
	{
		AnimationNodes.clear();
		SharedPtr<JsonData> val = jsonData->GetMember(NAME_KEY);
		if (val)
		{
			Name = val->GetString();
		}
		val = jsonData->GetMember(DURATION_KEY);
		if (val)
		{
			Duration = val->GetFloat();
		}
		val = jsonData->GetMember(TICKS_PER_SECOND_KEY);
		if (val)
		{
			TicksPerSecond = val->GetFloat();
		}
		val = jsonData->GetMember(ANIMATION_NODES_KEY);
		if (val)
		{
			u32 elementCnt = val->GetSize();
			for (u32 i = 0; i < elementCnt; ++i)
			{
				SharedPtr<JsonData> animVal = val->GetJsonDataFromIndex(i);
				AnimationNode animNode;

				animNode.NodeName = animVal->GetMember(NODE_NAME_KEY)->GetString();

				// Location
				{
					SharedPtr<JsonData> locationValuesJson = animVal->GetMember(LOCATION_VALUES_KEY);
					SharedPtr<JsonData> locationTimesJson  = animVal->GetMember(LOCATION_TIMES_KEY);
					if (locationValuesJson != nullptr && locationTimesJson != nullptr)
					{
						animNode.LocationValues.resize(locationValuesJson->GetSize());
						animNode.LocationTimes.resize(locationTimesJson->GetSize());
						for (u32 i = 0; i < animNode.LocationValues.size(); ++i)
						{
							animNode.LocationValues[i] = locationValuesJson->GetJsonDataFromIndex(i)->GetVector3();
							animNode.LocationTimes[i] = locationTimesJson->GetJsonDataFromIndex(i)->GetFloat();
						}
					}
				}
				// Rotation
				{
					SharedPtr<JsonData> rotationValuesJson = animVal->GetMember(ROTATION_VALUES_KEY);
					SharedPtr<JsonData> rotationTimesJson  = animVal->GetMember(ROTATION_TIMES_KEY);
					if (rotationValuesJson != nullptr && rotationTimesJson != nullptr)
					{
						animNode.RotationValues.resize(rotationValuesJson->GetSize());
						animNode.RotationTimes.resize(rotationTimesJson->GetSize());
						for (u32 i = 0; i < animNode.RotationValues.size(); ++i)
						{
							animNode.RotationValues[i] = rotationValuesJson->GetJsonDataFromIndex(i)->GetQuaternion();
							animNode.RotationTimes[i] = rotationTimesJson->GetJsonDataFromIndex(i)->GetFloat();
						}
					}
				}
				// Scale
				{
					SharedPtr<JsonData> scaleValuesJson = animVal->GetMember(SCALE_VALUES_KEY);
					SharedPtr<JsonData> scaleTimesJson = animVal->GetMember(SCALE_TIMES_KEY);
					if (scaleValuesJson != nullptr && scaleTimesJson != nullptr)
					{
						animNode.ScaleValues.resize(scaleValuesJson->GetSize());
						animNode.ScaleTimes.resize(scaleTimesJson->GetSize());
						for (u32 i = 0; i < animNode.ScaleValues.size(); ++i)
						{
							animNode.ScaleValues[i] = scaleValuesJson->GetJsonDataFromIndex(i)->GetVector3();
							animNode.ScaleTimes[i] = scaleTimesJson->GetJsonDataFromIndex(i)->GetFloat();
						}
					}
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
		auto filePath = PathHelper::CombinePath(path, stock.Name) + JG_ASSET_FORMAT;

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
		if (AssetHelper::GetResourcePath(path, &absolutePath, &resourcePath) == false)
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
		if (AssetHelper::GetResourcePath(path, &absolutePath, &resourcePath) == false)
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
		if (AssetHelper::GetResourcePath(originAssetData->Path, &absolutePath, &resourcePath) == false)
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
		if (AssetHelper::GetResourcePath(reName, &absolutePath, &resourcePath) == false)
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
		case EAssetFormat::Skeletal:
		{
			SkeletalAssetStock stock;
			stock.LoadJson(assetVal);
			auto sAsset = LoadData->Asset->As<Asset<Skeletone>>();
			if (sAsset != nullptr)
			{
				sAsset->mData->SetSkeletalStock(stock);
			}
			else return false;
			break;
		}
		case EAssetFormat::AnimationClip:
		{
			AnimationClipAssetStock stock;
			stock.LoadJson(assetVal);
			auto aAsset = LoadData->Asset->As<Asset<AnimationClip>>();
			if (aAsset != nullptr)
			{
				aAsset->mData->SetAnimationClipStock(stock);
			}
			else return false;
			break;
		}
		case EAssetFormat::GameWorld:
		default:
			JG_LOG_ERROR("{0} AssetFormat is not supported in LoadAsset", (int)assetFormat);
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
					JG_LOG_ERROR("Asset Load Fail  : {0}", iter->second->Path);
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
					JG_LOG_INFO("Asset Load Success : {0}", iter->second->Path);
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
				JG_LOG_INFO("Asset Loading... : {0}", loadData->Path);
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
					JG_LOG_INFO("UnLoad Asset : {0}", unLoadData.Asset->GetAssetPath());

				}
				else
				{
					JG_LOG_INFO("UnLoad ReadWrite Asset : {0}", unLoadData.Asset->GetAssetPath());
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
			textureAsset->mData->SetTextureMemory((const jbyte*)textureStock->Pixels.data(), textureStock->Width, textureStock->Height, textureStock->Channels, textureStock->PixelPerUnit);
			break;
		case EAssetFormat::CubeMap:
			textureAsset->mData->SetTextureMemory(
				(const jbyte*)textureStock->Pixels.data(), textureStock->Width, textureStock->Height, textureStock->Channels, textureStock->PixelPerUnit, 
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
						JG_LOG_WARN("Failed {0} 's Param {1} Set Int", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_int2:
				{
					JVector2Int int2_value = value->GetVector2Int();
					if (materialAsset->mData->SetInt2(name, int2_value) == false)
					{
						JG_LOG_WARN("Failed {0} 's Param {1} Set Int2", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_int3:
				{
					JVector3Int int3_value = value->GetVector3Int();
					if (materialAsset->mData->SetInt3(name, int3_value) == false)
					{
						JG_LOG_WARN("Failed {0} 's Param {1} Set Int3", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_int4:
				{
					JVector4Int int4_value = value->GetVector4Int();
					if (materialAsset->mData->SetInt4(name, int4_value) == false)
					{
						JG_LOG_WARN("Failed {0} 's Param {1} Set Int4", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_uint:
				{
					u32 uint_value = value->GetUint32();
					if (materialAsset->mData->SetUint(name, uint_value) == false)
					{
						JG_LOG_WARN("Failed {0} 's Param {1} Set Uint", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_uint2:
				{
					JVector2Uint uint2_value = value->GetVector2Uint();
					if (materialAsset->mData->SetUint2(name, uint2_value) == false)
					{
						JG_LOG_WARN("Failed {0} 's Param {1} Set Uint2", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_uint3:
				{
					JVector3Uint uint3_value = value->GetVector3Uint();
					if (materialAsset->mData->SetUint3(name, uint3_value) == false)
					{
						JG_LOG_WARN("Failed {0} 's Param {1} Set Uint3", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_uint4:
				{
					JVector4Uint uint4_value = value->GetVector4Uint();
					if (materialAsset->mData->SetUint4(name, uint4_value) == false)
					{
						JG_LOG_WARN("Failed {0} 's Param {1} Set Uint4", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_float:
				{
					f32 f32_value = value->GetFloat();
					if (materialAsset->mData->SetFloat(name, f32_value) == false)
					{
						JG_LOG_WARN("Failed {0} 's Param {1} Set Float", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_float2:
				{
					JVector2 float2_value = value->GetVector2();
					if (materialAsset->mData->SetFloat2(name, float2_value) == false)
					{
						JG_LOG_WARN("Failed {0} 's Param {1} Set Float2", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_float3:
				{
					JVector3 float3_value = value->GetVector3();
					if (materialAsset->mData->SetFloat3(name, float3_value) == false)
					{
						JG_LOG_WARN("Failed {0} 's Param {1} Set Float3", materialStock->Name, name);
					}
				}
				break;
				case JG::EShaderDataType::_float4:
				{
					JVector4 float4_value = value->GetVector4();
					if (materialAsset->mData->SetFloat4(name, float4_value) == false)
					{
						JG_LOG_WARN("Failed {0} 's Param {1} Set Float4", materialStock->Name, name);
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
								JG_LOG_WARN("Failed {0} 's Param {1} Set Float4", materialStock->Name, name);
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
		case EAssetFormat::AnimationClip: return CreateSharedPtr<Asset<AnimationClip>>(assetID, path, assetFormat);
		case EAssetFormat::Skeletal: return CreateSharedPtr<Asset<Skeletone>>(assetID, path, assetFormat);
		}


		return nullptr;
	}



	bool AssetDataBase::WriteAsset(const String& path, EAssetFormat format, SharedPtr<Json> json)
	{
		String absolutePath;
		String resourcePath;
		if (AssetHelper::GetResourcePath(path, &absolutePath, &resourcePath) == false)
		{
			JG_LOG_ERROR("Resource must exist in AssetPath : {0}", path);
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
			JG_LOG_ERROR("Fail Write Asset : {0}", path);
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
		if (AssetHelper::GetResourcePath(path, &absolutePath, &resourcePath) == false)
		{
			JG_LOG_ERROR("Resource must exist in AssetPath : {0}", path);
			return false;
		}
		std::lock_guard<std::mutex> lock(mAssetRWMutex);
		std::ifstream fin;
		fin.open(absolutePath);
		if (fin.is_open() == false)
		{
			JG_LOG_ERROR("Failed Read Asset : {0}", path);
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
		if (AssetHelper::GetResourcePath(path, &absolutePath, &resourcePath) == false)
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