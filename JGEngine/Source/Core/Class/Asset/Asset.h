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
	struct JGBone
	{

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
	public:
		String Name;
		bool   IsSkinned = false;
		List<String>           SubMeshNames;
		List<List<JGVertex>>   Vertices;
		List<List<u32>>		   Indices;
		JBBox BoundingBox;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual ~StaticMeshAssetStock() = default;
	public:
		virtual EAssetFormat GetAssetFormat() const override {
			return EAssetFormat::Mesh;
		}
	};


	class MaterialAssetStock : public IAssetStock
	{
	public:
		String Name;
		String ShaderTemplate;
		String ShaderScript;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual ~MaterialAssetStock() = default;
	public:
		virtual EAssetFormat GetAssetFormat() const override {
			return EAssetFormat::Material;
		}
	};
	

#define ASSET_NULL_ID -1

	class AssetID
	{
		friend class AssetDataBase;
		friend class AssetManager;
	private:
		u64  Origin = ASSET_NULL_ID;
		u64  ID     = ASSET_NULL_ID;
		char ResourcePath[256] = { 0, };
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

	class IAsset : public IJGObject
	{
	public:
		virtual u64 GetAssetID() const = 0;
		virtual const String& GetAssetFullPath() const = 0;
		virtual const String& GetAssetPath() const     = 0;
		virtual const String& GetAssetName() const     = 0;
		virtual const String& GetExtension() const = 0;
	public:
		virtual ~IAsset() = default;
	};


	class AssetInspectorGUI
	{
		template<class T>
		friend class Asset;

	private:
		static void InspectorGUI(IAsset* targetAsset);
	private:
		static void Material_InsepctorGUI(Asset<IMaterial>* targetAsset);
	};


	template<class T>
	class Asset : public IAsset
	{
		JGCLASS
		friend class AssetDataBase;
		friend class AssetManager;
		u64    mAssetID = ASSET_NULL_ID;
		String mAssetPath;
		String mAssetFullPath;
		String mExtension;
		String mName;

		SharedPtr<T> mData = nullptr;
	public:
		Asset(const String& assetPath)
		{
			fs::path p(assetPath);
			fs::path contentsPath = fs::absolute(Application::GetAssetPath()).string();
			mAssetID = (u64)this;
			mAssetFullPath = fs::absolute(assetPath).string();
			mAssetPath	   = ReplaceAll(mAssetFullPath, contentsPath.string(), "Asset");
			mExtension     = p.extension().string();
			mName = ReplaceAll(p.filename().string() ,mExtension, "");
		}
	public:
		virtual u64 GetAssetID() const override
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
		SharedPtr<T> Get() const {
			return mData;
		}
		virtual void OnInspectorGUI() override {
			AssetInspectorGUI::InspectorGUI(this);
		}
	public:
		virtual ~Asset() = default;
	};








	class AssetManager;
	class AssetDataBase : public GlobalSingleton<AssetDataBase>
	{
		friend class Application;
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
			u32               RefCount  = 0;
			EAssetDataState   State     = EAssetDataState::None;
			SharedPtr<IAsset> Asset		= nullptr;
		};
		// �ε����� ���� ������
		struct AssetLoadCompeleteData;
		struct AssetLoadData
		{
			AssetID ID;
			char Path[256] = { 0, };
			SharedPtr<IAsset> Asset = nullptr;
			SharedPtr<IAssetStock> Stock = nullptr;
			std::function<void(AssetLoadCompeleteData*)> OnComplete;
		};
		// �ε� �Ϸ�� ���� ������
		struct AssetLoadCompeleteData
		{
			AssetID ID;
			SharedPtr<IAsset> Asset = nullptr;
			SharedPtr<IAssetStock> Stock = nullptr;
			std::function<void(AssetLoadCompeleteData*)> OnComplete;
		};
		// ��ε� ���� ���� ������
		struct AssetUnLoadData
		{
			AssetID ID;
			SharedPtr<IAsset> Asset;
			u32 FrameCount  = 0;
		};



		// ���� ������ Pool;
		std::unordered_map<AssetID, UniquePtr<AssetData>, AssetIDHash> mAssetDataPool;
		Dictionary<String, AssetData*>			  mOriginAssetDataPool;
		Dictionary<String, EAssetFormat>		  mOriginAssetFormatPool;

		// ���� �ε�/��ε� ���� ���� ������ 
		Queue<AssetLoadData>   mLoadAssetDataQueue;
		Queue<AssetLoadCompeleteData>   mLoadCompeleteAssetDataQueue;
		Queue<AssetUnLoadData> mUnLoadAssetDataQueue;


	
		u64 mAssetIDOffset = 0;
		Queue<u64> mAssetIDQueue;

		const u32 mMaxLoadAssetDataCount   = 10;
		const u32 mMaxUnLoadAssetDataCount = 10;

		SharedPtr<ScheduleHandle> mAssetLoadScheduleHandle = nullptr;
		SharedPtr<ScheduleHandle> mAssetUnLoadScheduleHandle = nullptr;

		std::mutex mCompeleteMutex;
		std::shared_mutex mAssetFormatMutex;



		Dictionary<AssetManager*, SharedPtr<AssetManager>> mAssetManagerPool;
	public:
		AssetDataBase();
		virtual ~AssetDataBase();
	public:
		SharedPtr<AssetManager> RequestAssetManager();
		void ReturnAssetManager(SharedPtr<AssetManager> assetManager);
	public:
		EAssetFormat GetAssetFormat(const String& path);
		AssetID LoadOriginAsset(const String& path);
		AssetID LoadReadWriteAsset(AssetID originID);
		void	UnLoadAsset(AssetID id);

		template<class T>
		Asset<T>* GetAsset(AssetID assetID) const
		{
			auto iter = mAssetDataPool.find(assetID);
			if (iter == mAssetDataPool.end())
			{
				return nullptr;
			}
			if (iter->second->State == EAssetDataState::None && iter->second->Asset->GetType() == JGTYPE(Asset<T>))
			{
				return static_cast<Asset<T>*>(iter->second->Asset.get());
			}
			return nullptr;
		}
		IAsset* GetIAsset(AssetID assetID) const 
		{
			auto iter = mAssetDataPool.find(assetID);
			if (iter == mAssetDataPool.end())
			{
				return nullptr;
			}
			if (iter->second->State == EAssetDataState::None)
			{
				return iter->second->Asset.get();
			}
			return nullptr;
		}
	private:
		AssetID RequestOriginAssetID(const String& resourcePath);
		AssetID RequestRWAssetID(AssetID originID);
		void LoadAssetInternal(AssetLoadData* LoadData);


		EScheduleResult LoadAsset_Update();
		EScheduleResult UnLoadAsset_Update();
		bool GetResourcePath(const String& path, String& out_absolutePath, String& out_resourcePath) const;
	private:
		void TextureAsset_OnCompelete(AssetLoadCompeleteData* data);

		// ��û �ޱ�

		// ���⿡ �񵿱�� ���� �ε� ���� ����
		// ���� ����
		// ������ ����
		// ������ ����


		// ������ ������ ������ ������ ����
		// GetOriginAsset
		// GetAsset( id �� �ҷ��´�. )
	};
}