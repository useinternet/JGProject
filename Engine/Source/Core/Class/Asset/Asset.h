#pragma once
#include "Common/Define.h"
#include "Common/Enum.h"
#include "Common/Abstract.h"
#include "Common/Type.h"
#include "Graphics/Resource.h"
#include "Class/Global/Scheduler.h"
#include "Class/FileIO.h"
namespace JG
{
	struct JGVertex
	{
		JVector3 Position;
		JVector2 Texcoord;
		JVector3 Normal;
		JVector3 Tangent;
		JVector3 Bitangent;


		static SharedPtr<InputLayout> GetInputLayout() {
			auto inputLayout = InputLayout::Create();
			inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
			inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);
			inputLayout->Add(EShaderDataType::_float3, "NORMAL", 0);
			inputLayout->Add(EShaderDataType::_float3, "TANGENT", 0);
			inputLayout->Add(EShaderDataType::_float3, "BITANGENT", 0);

			return inputLayout;
		}
	public:
		void MakeJson(SharedPtr<JsonData> jsonData) const 
		{
			auto& val = jsonData->GetValue();
			val.SetArray();
			val.PushBack(Position.x, jsonData->GetJsonAllocator());
			val.PushBack(Position.y, jsonData->GetJsonAllocator());
			val.PushBack(Position.z, jsonData->GetJsonAllocator());
			val.PushBack(Texcoord.x, jsonData->GetJsonAllocator());
			val.PushBack(Texcoord.y, jsonData->GetJsonAllocator());
			val.PushBack(Normal.x, jsonData->GetJsonAllocator());
			val.PushBack(Normal.y, jsonData->GetJsonAllocator());
			val.PushBack(Normal.z, jsonData->GetJsonAllocator());
			val.PushBack(Tangent.x, jsonData->GetJsonAllocator());
			val.PushBack(Tangent.y, jsonData->GetJsonAllocator());
			val.PushBack(Tangent.z, jsonData->GetJsonAllocator());
			val.PushBack(Bitangent.x, jsonData->GetJsonAllocator());
			val.PushBack(Bitangent.y, jsonData->GetJsonAllocator());
			val.PushBack(Bitangent.z, jsonData->GetJsonAllocator());
		}
		void LoadJson(SharedPtr<JsonData> jsonData)
		{
			auto& val = jsonData->GetValue();
			i32 index = 0;
			for (auto& value : val.GetArray())
			{
				f32 f = value.GetFloat();
				if (index < 3)
				{
					Position[index] = f;
				}
				else if (index < 5)
				{
					Texcoord[index - 3] = f;
				}
				else if (index < 8)
				{
					Normal[index - 5] = f;
				}
				else if (index < 11)
				{
					Tangent[index - 8] = f;
				}
				else
				{
					Bitangent[index - 11] = f;
				}
				++index;
			}
		}
	};

	struct JGQuadVertex
	{
		JVector3 Position;
		JVector2 Texcoord;


		static SharedPtr<InputLayout> GetInputLayout() {
			auto inputLayout = InputLayout::Create();
			inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
			inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);
			return inputLayout;
		}
	public:
		void MakeJson(SharedPtr<JsonData> jsonData) const 
		{
			jsonData->AddMember("Position", Position);
			jsonData->AddMember("Texcoord", Texcoord);
		}
		void LoadJson(SharedPtr<JsonData> jsonData)
		{
			auto val = jsonData->GetMember("Position");
			if (val)
			{
				Position = val->GetVector3();
			}
			val = jsonData->GetMember("Texcoord");
			if (val)
			{
				Texcoord = val->GetVector2();
			}
		}
	};
	struct JGDebugVertex
	{
		JVector3 Position;
		JGDebugVertex() = default;
		JGDebugVertex(f32 x, f32 y, f32 z) : Position(x, y, z) {}
		JGDebugVertex(const JVector3& p) : Position(p) {}

		static SharedPtr<InputLayout> GetInputLayout() {
			auto inputLayout = InputLayout::Create();
			inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);


			return inputLayout;
		}
	};
	struct JGBoneVertex
	{
		JVector4Int BoneIDs;
		JVector4	BoneWeights;
	public:
		void MakeJson(SharedPtr<JsonData> jsonData) const
		{
			auto& val = jsonData->GetValue();
			val.SetArray();
			val.PushBack(BoneIDs.x, jsonData->GetJsonAllocator());
			val.PushBack(BoneIDs.y, jsonData->GetJsonAllocator());
			val.PushBack(BoneIDs.z, jsonData->GetJsonAllocator());
			val.PushBack(BoneIDs.w, jsonData->GetJsonAllocator());
			val.PushBack(BoneWeights.x, jsonData->GetJsonAllocator());
			val.PushBack(BoneWeights.y, jsonData->GetJsonAllocator());
			val.PushBack(BoneWeights.z, jsonData->GetJsonAllocator());
			val.PushBack(BoneWeights.w, jsonData->GetJsonAllocator());
		}
		void LoadJson(SharedPtr<JsonData> jsonData)
		{
			auto& val = jsonData->GetValue();
			i32 index = 0;
			for (const auto& value : val.GetArray())
			{
				if (index < 4)
				{
					i32 i = value.GetInt();
					BoneIDs[index] = i;
				}
				else
				{
					f32 f = value.GetFloat();
					BoneWeights[index - 4] = f;
				}
				++index;
			}
		}
	};
	struct JGBoneOffsetData
	{
		u32 ID;
		JMatrix Offset;
	public:
		void MakeJson(SharedPtr<JsonData> jsonData) const
		{
			auto& val = jsonData->GetValue();
			val.SetArray();
			val.PushBack(ID, jsonData->GetJsonAllocator());

			for (int col = 0; col < 4; ++col)
			{
				for (int raw = 0; raw < 4; ++raw)
				{
					val.PushBack(Offset.Get_C(col, raw), jsonData->GetJsonAllocator());
				}
			}
		}
		void LoadJson(SharedPtr<JsonData> jsonData)
		{
			auto& val = jsonData->GetValue();
			i32 index = 0;
			for (const auto& value : val.GetArray())
			{
				if (index == 0)
				{
					i32 i = value.GetInt();
					ID = i;
				}
				else
				{
					i32 col = (index - 1) / 4;
					i32 row = (index - 1) % 4;
					
					f32 f = value.GetFloat();
					Offset.Get(col, row) = f;
				}
				++index;
			}
		}
	};

	class IAssetStock : public IJson
	{
	public:
		virtual ~IAssetStock() = default;
		virtual EAssetFormat GetAssetFormat() const = 0;
	};

	class TextureAssetStock :  public IAssetStock
	{
	public:
		String Name;
		i32 Width    = 0;
		i32 Height   = 0;
		i32 Channels = 0;
		u32 PixelPerUnit = 100;
		u32 OriginPixelSize = 0;
		List<jbyte> Pixels;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual ~TextureAssetStock() = default;
	public:
		virtual EAssetFormat GetAssetFormat() const  override {
			return EAssetFormat::Texture;
		}
	};
	class StaticMeshAssetStock : public IAssetStock
	{
		static const constexpr char* NAME_KEY			= "Name";
		static const constexpr char* BOUNDING_BOX_KEY	= "BoundingBox";
		static const constexpr char* SUBMESHS_KEY		= "SubMeshs";
		static const constexpr char* VERTICES_KEY		= "Vertices";
		static const constexpr char* BONE_VERTICES_KEY	= "BoneVertices";
		static const constexpr char* BONE_OFFSETDATAS_KEY   = "BoneOffsetDatas";
		static const constexpr char* INDICES_KEY		= "Indices";
	public:
		String Name;
		List<String>			 SubMeshNames;
		List<List<JGVertex>>     Vertices;
		List<List<JGBoneVertex>> BoneVertices;
		List<List<JGBoneOffsetData>> BoneOffsetDatas;
		List<List<u32>>			 Indices;
		JBBox BoundingBox;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual ~StaticMeshAssetStock() = default;
	public:
		virtual EAssetFormat GetAssetFormat() const override {
			return BoneVertices.empty() ? EAssetFormat::Mesh : EAssetFormat::SkeletalMesh;
		}
	};
	class SkeletalAssetStock : public IAssetStock
	{
		static const constexpr char* NAME_KEY				= "Name";
		static const constexpr char* ROOT_BONE_NODE_KEY		= "RootBoneNode";
		static const constexpr char* BONE_NODES_KEY			= "BoneNodes";
		static const constexpr char* ROOT_OFFSET_KEY		= "RootOffset";
		static const constexpr char* BONE_NODE_ID_KEY		= "BoneNodeID";
		static const constexpr char* BONE_NODE_NAME_KEY		= "BoneNodeName";
		static const constexpr char* PARENT_BONDE_NODE_KEY	= "ParentBoneNode";
		static const constexpr char* CHILD_BONE_NODES_KEY	= "ChildNodes";
		static const constexpr char* TRANSFORM_KEY			= "Transform";
	public:
		struct BoneNode
		{
			u32 ID;
			String Name;

			u32 ParentNode;
			List<u32> ChildNodes;

			JMatrix Transform;
		};
	public:
		String Name;
		u32	   RootBoneNode;
		List<BoneNode> BoneNodes;
		JMatrix RootOffset;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual ~SkeletalAssetStock() = default;
	public:
		virtual EAssetFormat GetAssetFormat() const override {
			return EAssetFormat::Skeletal;
		}
	};


	enum class EAnimationParameterType;
	enum class EAnimationClipFlags;
	enum class EAnimationCondition;
	class AnimationAssetStock : public IAssetStock
	{
		static const constexpr char* ROOT_NAME_KEY = "RootName";
		static const constexpr char* ANIM_CLIP_LIST_KEY = "AnimClipList";
		static const constexpr char* ANIM_PARAM_LIST_KEY = "AimParamList";

		static const constexpr char* ANIM_CLIP_NAME_KEY = "AnimClipName";
		static const constexpr char* ANIM_CLIP_ASSETPATH_KEY = "AnimClipAssetPath";
		static const constexpr char* ANIM_CLIP_ANIM_FLAGS_KEY = "AnimClipFlags";

		static const constexpr char* ANIM_PARAM_NAME_KEY = "Name";
		static const constexpr char* ANIM_PARAM_TYPE_KEY = "Type";
		static const constexpr char* ANIM_PARAM_DATA_KEY = "Data";


		static const constexpr char* ANIM_TRANSITION_LIST_KEY = "TransitionList";

		static const constexpr char* ANIM_TRANSITION_PREV_NODE_NAME_KEY = "PrevNodeName";
		static const constexpr char* ANIM_TRANSITION_NEXT_NODE_NAME_KEY = "NextNodeName";
		static const constexpr char* ANIM_TRANSITION_DURATION_KEY = "TransitionDuration";
		static const constexpr char* ANIM_TRANSITION_CONDITION_LIST_KEY = "TransitionConditionList";

		static const constexpr char* ANIM_CONDITION_NAME_KEY = "Name";
		static const constexpr char* ANIM_CONDITION_TYPE_KEY = "Condition";
		static const constexpr char* ANIM_CONDITION_DATA_KEY = "Data";
	public:
		struct ParameterData
		{
			EAnimationParameterType Type;
			String Name;
			List<jbyte> Data;
		};
		struct AnimationClipInfo
		{
			String Name;
			String AssetPath;
			EAnimationClipFlags Flags;
		};
		struct AnimationTransitionConditionInfo
		{
			String ParameterName;
			EAnimationCondition Condition;
			List<jbyte> Data;
		};
		struct AnimationTransitionInfo
		{
			String PrevName;
			String NextName;
			f32 TransitionDuration = 0.0f;
			List<AnimationTransitionConditionInfo> Transitions;
		};
		String RootName;
		List<AnimationClipInfo> AnimClips;
		Dictionary<String, ParameterData> Parameters;
		List<AnimationTransitionInfo> TransitionInfos;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual ~AnimationAssetStock() = default;
	public:
		virtual EAssetFormat GetAssetFormat() const override {
			return EAssetFormat::Animation;
		}

	};

	class AnimationClipAssetStock : public IAssetStock
	{
		static const constexpr char* NAME_KEY	  = "Name";
		static const constexpr char* DURATION_KEY = "Duration";
		static const constexpr char* TICKS_PER_SECOND_KEY = "TicksPerSecond";
		static const constexpr char* ANIMATION_NODES_KEY = "AnimationNodes";

		static const constexpr char* NODE_NAME_KEY = "NodeName";
		static const constexpr char* LOCATION_VALUES_KEY = "LocationValues";
		static const constexpr char* ROTATION_VALUES_KEY = "RotationValues";
		static const constexpr char* SCALE_VALUES_KEY = "ScaleValues";

		static const constexpr char* LOCATION_TIMES_KEY = "LocationTimes";
		static const constexpr char* ROTATION_TIMES_KEY = "RotationTimes";
		static const constexpr char* SCALE_TIMES_KEY = "ScaleTimes";
	public:
		struct AnimationNode
		{
			String NodeName;

			List<JVector3>	  LocationValues;
			List<JQuaternion> RotationValues;
			List<JVector3>    ScaleValues;

			List<f32> LocationTimes;
			List<f32> RotationTimes;
			List<f32> ScaleTimes;
		};
	public:
		String Name;
		f32 Duration;
		f32 TicksPerSecond;
		Dictionary<String, AnimationNode> AnimationNodes;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual ~AnimationClipAssetStock() = default;
	public:
		virtual EAssetFormat GetAssetFormat() const override {
			return EAssetFormat::AnimationClip;
		}
	};

	
	class AnimationBlendSpace1DStock : public IAssetStock
	{
	public:
		struct AnimClipData : public IJson
		{
			String Name;
			String AssetPath;
			f32    Value;


			// IJson을(를) 통해 상속됨
			virtual void MakeJson(SharedPtr<JsonData> jsonData) const override {
				jsonData->AddMember("Name", Name);
				jsonData->AddMember("AssetPath", AssetPath);
				jsonData->AddMember("Value", Value);
			}

			virtual void LoadJson(SharedPtr<JsonData> jsonData) override {
				Name = jsonData->GetMember("Name")->GetString();
				AssetPath = jsonData->GetMember("AssetPath")->GetString();
				Value = jsonData->GetMember("Value")->GetFloat();
			}

		};
	public:
		String Name;
		String XParamName;
		JVector2 MinMaxValue;
		List<AnimClipData> AnimClipDatas;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual ~AnimationBlendSpace1DStock() = default;
	public:
		virtual EAssetFormat GetAssetFormat() const override {
			return EAssetFormat::AnimationClip;
		}
	};
	class AnimationBlendSpaceStock : public IAssetStock
	{
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual ~AnimationBlendSpaceStock() = default;
	public:
		virtual EAssetFormat GetAssetFormat() const override {
			return EAssetFormat::AnimationBlendSpace;
		}
	};

	class MaterialAssetStock : public IAssetStock
	{
	public:
		String Name;
		String ShaderTemplate;
		String ShaderScript;
		Dictionary<String, std::pair<EShaderDataType, SharedPtr<JsonData>>> MaterialDatas;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual ~MaterialAssetStock() = default;
	public:
		virtual EAssetFormat GetAssetFormat() const override {
			return EAssetFormat::Material;
		}
		static bool Write(const String& path, const MaterialAssetStock& stock);
	};
	

#define ASSET_NULL_ID -1
	class AssetID
	{
		// 참조하고있는 에셋
		friend class AssetDataBase;
		friend class AssetManager;

		template<class T>
		friend class Asset;

	private:
		u64  Origin = ASSET_NULL_ID;
		u64  ID     = ASSET_NULL_ID;
		char ResourcePath[256] = { 0, };
	public:
		virtual ~AssetID() = default;
	public:
		u64 GetID() const {
			return ID;
		}
		u64 GetOriginID() const {
			return Origin;
		}
		bool IsOrigin() const {
			return Origin == ID;
		}
		bool IsValid() const {
			return ID != ASSET_NULL_ID;
		}
	public:
		bool operator==(const AssetID& id) const
		{
			return ID == id.GetID();
		}
		bool operator!=(const AssetID& id) const
		{
			return ID != id.GetID();
		}
	};

	template<class T>
	class Asset;

	class IAsset : public IJGObject
	{
		JGCLASS
	protected:
		virtual void Set(AssetID assetID, const String& assetPath, EAssetFormat assetFormat) = 0;
	public:
		virtual AssetID GetAssetID() const = 0;
		virtual const String& GetAssetFullPath() const = 0;
		virtual const String& GetAssetPath() const     = 0;
		virtual const String& GetAssetName() const     = 0;
		virtual const String& GetExtension() const = 0;
		virtual EAssetFormat GetAssetFormat() const = 0;
	public:
		virtual ~IAsset() = default;
	private:
		friend class AssetDataBase;
		virtual SharedPtr<IAsset> Copy() const = 0;
	protected:
		const String& GetAssetRootPath() const;
	};

	template<class T>
	class Asset : public IAsset
	{
		JGCLASS
		friend class AssetDataBase;
		friend class AssetManager;
		AssetID mAssetID;
		String  mAssetPath;
		String  mAssetFullPath;
		String  mExtension;
		String  mName;
		EAssetFormat mAssetFormat;
		SharedPtr<T> mData = nullptr;
	public:
		Asset(AssetID assetID, const String& assetPath, EAssetFormat assetFormat)
		{
			Set(assetID, assetPath, assetFormat);
			mData = T::Create(mAssetPath);
		}
	private:
		virtual void Set(AssetID assetID, const String& assetPath, EAssetFormat assetFormat) override
		{
			fs::path p(assetPath);
			String contentsPath = StringHelper::ReplaceAll(fs::absolute(GetAssetRootPath()).string(), "\\", "/");
			mAssetID = assetID;
		
			mAssetFullPath = fs::absolute(assetPath).string(); mAssetFullPath = StringHelper::ReplaceAll(mAssetFullPath, "\\", "/");
			mAssetPath = StringHelper::ReplaceAll(mAssetFullPath, contentsPath, "Asset");
			strcpy(mAssetID.ResourcePath, mAssetPath.c_str());
			mExtension	 = p.extension().string();
			mName		 = StringHelper::ReplaceAll(p.filename().string(), mExtension, "");
			mAssetFormat = assetFormat;
			if (IsValid() == true)
			{
				mData->SetName(mName);
			}
		}
	public:
		virtual AssetID GetAssetID() const override
		{
			return mAssetID;
		}
		virtual const String& GetAssetFullPath() const override
		{
			return mAssetFullPath;
		}
		virtual const String& GetAssetPath() const override
		{
			return mAssetPath;
		}
		virtual const String& GetAssetName() const override
		{
			return mName;
		}
		virtual const String& GetExtension() const override
		{
			return mExtension;
		}
		virtual EAssetFormat GetAssetFormat() const override
		{
			return mAssetFormat;
		}
		bool IsValid() const {
			return Get() != nullptr && Get()->IsValid();
		}
		SharedPtr<T> Get() const {
			return mData;
		}
	public:
		virtual ~Asset() = default;
	private:
		virtual SharedPtr<IAsset> Copy() const override {
			auto asset = CreateSharedPtr<Asset<T>>(mAssetID, mAssetFullPath, mAssetFormat);
			asset->mData = mData;
			return asset;
		}
	};







	class AssetManager;
	class IMaterial;
	class Skeletone;
	class AssetDataBase : public IGlobalSingleton<AssetDataBase>
	{
	private:
		enum class EAssetDataState
		{
			None,
			Loading,
		};
		struct AssetIDHash
		{
			std::size_t operator()(const AssetID& id) const {
				return id.ID;
			}
		};
		struct AssetData
		{
			AssetID ID;
			String  Path;
			EAssetDataState   State     = EAssetDataState::None;
			SharedPtr<IAsset> Asset		= nullptr;
		};
		// 로딩중인 에셋 데이터
		struct AssetLoadCompeleteData;
		class AssetLoadData : public IJGObject
		{
			JGCLASS
		public:
			AssetID ID;
			char Path[256] = { 0, };
			SharedPtr<IAsset>      Asset = nullptr;
			SharedPtr<IAssetStock> Stock = nullptr;
			SharedPtr<Json>        Json = nullptr;
			std::function<void(AssetLoadCompeleteData*)> OnComplete;

		};
		// 로드 완료된 에셋 데이터
		struct AssetLoadCompeleteData
		{
			AssetID ID;
			SharedPtr<Json>   Json  = nullptr;
			SharedPtr<IAsset> Asset = nullptr;
			SharedPtr<IAssetStock> Stock = nullptr;
			std::function<void(AssetLoadCompeleteData*)> OnComplete;
		};
		// 언로드 중인 에셋 데이터
		struct AssetUnLoadData
		{
			AssetID ID;
			SharedPtr<IAsset> Asset;
			u32 FrameCount  = 0;
		};


		// 에셋 데이터 Pool;
		std::unordered_map<AssetID, UniquePtr<AssetData>, AssetIDHash> mAssetDataPool;
		std::unordered_map<AssetID, std::unordered_set<AssetID, AssetIDHash>, AssetIDHash> mAssetDependencies;
		Dictionary<String, AssetData*>			  mOriginAssetDataPool;

		// 현재 로딩/언로드 중인 에셋 데이터 
		Queue<SharedPtr<AssetLoadData>>   mLoadAssetDataQueue;
		Queue<AssetLoadCompeleteData>   mLoadCompeleteAssetDataQueue;
		Queue<AssetUnLoadData> mUnLoadAssetDataQueue;

		u64 mAssetIDOffset = 0;
		Queue<u64> mAssetIDQueue;

		u32 mMaxLoadAssetDataCount   = 10;
		const u32 mMaxUnLoadAssetDataCount = 10;

		List<SharedPtr<ScheduleHandle>> mAyncLoadAssetHandleList;
		SharedPtr<ScheduleHandle> mAssetLoadScheduleHandle = nullptr;
		SharedPtr<ScheduleHandle> mAssetUnLoadScheduleHandle = nullptr;


		std::mutex mAssetLoadMutex;
		std::mutex mCompeleteMutex;
		std::mutex mAssetRWMutex;



		Dictionary<AssetManager*, SharedPtr<AssetManager>> mAssetManagerPool;
	public:
		AssetDataBase();
		virtual ~AssetDataBase();
	public:
		SharedPtr<AssetManager> RequestAssetManager();
		void ReturnAssetManager(SharedPtr<AssetManager> assetManager);
	public:
		AssetID GetAssetOriginID(const String& path);
		SharedPtr<IAsset> LoadOriginAsset(const String& path);
		SharedPtr<IAsset> LoadOriginAssetImmediate(const String& path);
		SharedPtr<IAsset> LoadReadWriteAsset(AssetID originID);

		template<class T>
		SharedPtr<Asset<T>> LoadOriginAsset(const String& path)
		{
			SharedPtr<IAsset> originAsset = LoadOriginAsset(path);
			if (originAsset == nullptr || originAsset->Is<Asset<T>>() == false)
			{
				return nullptr;
			}
			return std::static_pointer_cast<Asset<T>>(originAsset);
		}

		template<class T>
		SharedPtr<Asset<T>> LoadOriginAssetImmediate(const String& path)
		{
			SharedPtr<IAsset> originAsset = LoadOriginAssetImmediate(path);
			if (originAsset == nullptr || originAsset->Is<Asset<T>>() == false)
			{
				return nullptr;
			}
			return std::static_pointer_cast<Asset<T>>(originAsset);
		}

		template<class T>
		SharedPtr<Asset<T>> LoadReadWriteAsset(AssetID originID)
		{
			SharedPtr<IAsset> rwAsset = LoadReadWriteAsset(originID);
			if (rwAsset == nullptr || rwAsset->Is<Asset<T>>() == false)
			{
				return nullptr;
			}
			return std::static_pointer_cast<Asset<T>>(rwAsset);
		}

		void UnLoadAsset(AssetID id);
		void RefreshAsset(AssetID originID, bool originImmediate = false, const String& reName = String());
		void RefreshAssetName(AssetID originID, const String& reName);
	private:
		AssetID RequestOriginAssetID();
		AssetID RequestRWAssetID(AssetID originID);
		bool LoadAssetInternal(AssetLoadData* LoadData);


		EScheduleResult LoadAsset_Update();
		void LoadCompeleteData_Update();
		void LoadAssetData_Update();

		EScheduleResult UnLoadAsset_Update();
		SharedPtr<IAsset> CreateAsset(AssetID assetID, const String& path);
	private:
		void TextureAsset_OnCompelete(AssetLoadCompeleteData* data);
		void MaterialAsset_OnCompelete(AssetLoadCompeleteData* data);
		void MeshAsset_OnCompelete(AssetLoadCompeleteData* data);
	public:

		bool WriteAsset(const String& path, EAssetFormat format, SharedPtr<Json> json);
		bool ReadAsset(const String& path, EAssetFormat* out_format, SharedPtr<Json>* json);
		EAssetFormat  GetAssetFormat(const String& path);
	};


}