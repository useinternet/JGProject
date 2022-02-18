#pragma once
#include "JGCore.h"

namespace JG
{
	class Renderer;
	class ITexture;
	class IStructuredBuffer;
	class IByteAddressBuffer;
	class RenderParamManager;

	class RenderParam
	{
	protected:
		RenderParamManager* mRenderParamManager = nullptr;
		bool      mIsValid = false;
		String    mName;

	protected:
		RenderParam() = default;
	public:
		virtual ~RenderParam() = default;
		
	public:
		bool IsVaild() const {
			return mIsValid && mRenderParamManager != nullptr;
		}
		const String& GetName() const {
			return mName;
		}
	};

	template<class T>
	class RP_GlobalVar : public RenderParam
	{
		T Value;
	public:
		virtual ~RP_GlobalVar() = default;
	public:
		bool SetValue(const T& value)
		{
			if (IsVaild() == false)
			{
				return false;
			}
			Value = value;
			mRenderParamManager->SetGlobalRenderParam<T>(GetName(), Value);
			return true;
		}
		const T& GetValue()
		{
			if (IsVaild() == false)
			{
				return Value;
			}
			mRenderParamManager->GetGlobalRenderParam<T>(GetName(), &Value);
			return Value;
		}
	public:
		static RP_GlobalVar<T> Create(const String& name, const T& initValue, RenderParamManager* rpManger)
		{
			RP_GlobalVar<T> val;
			val.mRenderParamManager = rpManger;
			val.mName = name;
			val.Value = initValue;

			if (rpManger->RegisterGlobalRenderParam(name, initValue) == false)
			{
				return RP_GlobalVar<T>();
			}
			val.mIsValid  = true;
			return val;
		}
		static RP_GlobalVar<T> Create(const String& name, const T& initValue, const T& minValue, const T& maxValue, RenderParamManager* rpManger)
		{
			RP_GlobalVar<T> val;
			val.mRenderParamManager = rpManger;
			val.mName = name;
			val.Value = initValue;

			if (rpManger->RegisterGlobalRenderParam(name, initValue, minValue, maxValue) == false)
			{
				return RP_GlobalVar<T>();
			}
			val.mIsValid = true;
			return val;
		}
		static RP_GlobalVar<T> Load(const String& name, RenderParamManager* rpManger)
		{
			if (rpManger == nullptr)
			{
				return RP_GlobalVar<T>();
			}
			T value;
			if (rpManger->GetGlobalRenderParam(name, &value) == false)
			{
				return RP_GlobalVar<T>();
			}
			RP_GlobalVar<T> val;
			val.mRenderParamManager = rpManger;
			val.mName     = name;
			val.Value     = value;
			val.mIsValid  = true;
			return val;
		}
	};


	class RP_Global_Bool : public RenderParam
	{
		bool Value;
	public:
		virtual ~RP_Global_Bool() = default;
	public:
		bool SetValue(bool value);
		bool GetValue();
	public:
		static RP_Global_Bool Create(const String& name, bool initValue, RenderParamManager* rpManger);
		static RP_Global_Bool Load(const String& name, RenderParamManager* rpManger);
	};


	class RP_Global_Tex : public RenderParam
	{
	public:
		virtual ~RP_Global_Tex() = default;
	public:
		bool SetValue(SharedPtr<ITexture> value);
		SharedPtr<ITexture> GetValue() const;
	public:
		static RP_Global_Tex Create(const String& name, SharedPtr<ITexture> initTex, RenderParamManager* rpManger, bool isPrivate = false);
		static RP_Global_Tex Load(const String& name, RenderParamManager* rpManger);
	};

	class RP_Global_SB : public RenderParam
	{
	public:
		virtual ~RP_Global_SB() = default;
	public:
		bool SetValue(SharedPtr<IStructuredBuffer> value);
		SharedPtr<IStructuredBuffer> GetValue() const;
	public:
		static RP_Global_SB Create(const String& name, SharedPtr<IStructuredBuffer> initSB, RenderParamManager* rpManger, bool isPrivate = false);
		static RP_Global_SB Load(const String& name, RenderParamManager* rpManger);
	};

	class RP_Global_BAB : public RenderParam
	{
	public:
		virtual ~RP_Global_BAB() = default;
	public:
		bool SetValue(SharedPtr<IByteAddressBuffer> value);
		SharedPtr<IByteAddressBuffer> GetValue() const;
	public:
		static RP_Global_BAB Create(const String& name, SharedPtr<IByteAddressBuffer> initBAB, RenderParamManager* rpManger, bool isPrivate = false);
		static RP_Global_BAB Load(const String& name, RenderParamManager* rpManger);
	};

	using RP_Global_Float  = RP_GlobalVar<float>;
	//using RP_Global_Float2 = RP_GlobalVar<JVector2>;
	//using RP_Global_Float3 = RP_GlobalVar<JVector3>;
	//using RP_Global_Float4 = RP_GlobalVar<JVector4>;
	using RP_Global_Int    = RP_GlobalVar<i32>;
	//using RP_Global_Int2   = RP_GlobalVar<JVector2Int>;
	//using RP_Global_Int3   = RP_GlobalVar<JVector3Int>;
	//using RP_Global_Int4   = RP_GlobalVar<JVector4Int>;












	class RenderParamManager
	{
	public:
		struct RenderParamInfo
		{
			List<jbyte> Value;
			List<jbyte> MinValue;
			List<jbyte> MaxValue;
			Type DataType;
			void Init(void* value, void* minValue, void* maxValue, u64 dataSize, Type type)
			{
				DataType = type;
				Value.resize(dataSize);
				memcpy(Value.data(), value, dataSize);

				if (minValue != nullptr)
				{
					MinValue.resize(dataSize);
					memcpy(MinValue.data(), minValue, dataSize);
				}
				if (maxValue != nullptr)
				{
					MaxValue.resize(dataSize);
					memcpy(MaxValue.data(), maxValue, dataSize);
				}
			}
			u64 GetDataSize() const {
				return Value.size();
			}
			const Type& GetDataType() const {
				return DataType;
			}
			template<class T>
			bool GetValue(T* out_data) const
			{
				if (out_data == nullptr || Value.size() != sizeof(T))
				{
					return false;
				}
				memcpy((void*)out_data, Value.data(), sizeof(T));
				return true;
			}
			template<class T>
			bool GetMinValue(T* out_data) const
			{
				if (out_data == nullptr || Value.size() != sizeof(T) || MinValue.empty())
				{
					return false;
				}
				memcpy((void*)out_data, MinValue.data(), sizeof(T));
				return true;
			}
			template<class T>
			bool GetMaxValue(T* out_data) const
			{
				if (out_data == nullptr || Value.size() != sizeof(T) || MaxValue.empty())
				{
					return false;
				}
				memcpy((void*)out_data, MaxValue.data(), sizeof(T));
				return true;
			}
			template<class T>
			void Set(const T& data)
			{
				if (sizeof(T) != Value.size())
				{
					return;
				}
				T src = data;
				T temp;
				if (GetMinValue<T>(&temp) == true)
				{
					src = Math::Max(src, temp);
				}
				if (GetMaxValue<T>(&temp) == true)
				{
					src = Math::Min(src, temp);
				}
				memcpy((void*)Value.data(), (void*)&src, Value.size());
			}
		};
	private:
		template<class T>
		using PriviableParamDic = Dictionary<String, std::pair<SharedPtr<T>, bool>>;
		template<class T>
		using PublicParamDic = Dictionary<String, T>;

		PublicParamDic<RenderParamInfo>		  mGlobalParamDic;
		PriviableParamDic<ITexture>			  mGlobalParamTexDic;
		PriviableParamDic<IStructuredBuffer>  mGlobalParamSBDic;
		PriviableParamDic<IByteAddressBuffer> mGlobalParamBABDic;

		std::mutex mRenderParamMutex;
	public:
		void ForEach(const std::function<void(const String&, const RenderParamInfo&)>& action)
		{
			if (action == nullptr)
			{
				return;
			}
			std::lock_guard<std::mutex> lock(mRenderParamMutex);
			for (auto& _pair : mGlobalParamDic)
			{
				action(_pair.first, _pair.second);
			}
		}
		void ForEach(const std::function<void(const String&, SharedPtr<ITexture>)>& action)
		{
			
			if (action == nullptr)
			{
				return;
			}
			std::lock_guard<std::mutex> lock(mRenderParamMutex);
			for (auto& _pair : mGlobalParamTexDic)
			{
				action(_pair.first, _pair.second.first);
			}
		}
	public:
		bool RegisterGlobalRenderParamTex(const String& name, SharedPtr<ITexture> initTexture = nullptr, bool isPrivate = false);
		bool SetGlobalRenderParamTex(const String& name, SharedPtr<ITexture> tex);
		SharedPtr<ITexture> GetGlobalRenderParamTex(const String& name);


		bool RegisterGlobalRenderParamSB(const String& name, SharedPtr<IStructuredBuffer> initSB = nullptr, bool isPrivate = false);
		bool SetGlobalRenderParamSB(const String& name, SharedPtr<IStructuredBuffer> sb);
		SharedPtr<IStructuredBuffer> GetGlobalRenderParamSB(const String& name);


		bool RegisterGlobalRenderParamBAB(const String& name, SharedPtr<IByteAddressBuffer> initBAB = nullptr, bool isPrivate = false);
		bool SetGlobalRenderParamBAB(const String& name, SharedPtr<IByteAddressBuffer> bab);
		SharedPtr<IByteAddressBuffer> GetGlobalRenderParamBAB(const String& name);

		template<class T>
		bool RegisterGlobalRenderParam(const String& name, const T& value, Type type = JGTYPE(T))
		{
			if (mGlobalParamDic.find(name) != mGlobalParamDic.end())
			{
				return false;
			}
			mGlobalParamDic[name].Init((void*)&value, nullptr, nullptr, sizeof(T), type);
			return true;
		}
		template<class T>
		bool RegisterGlobalRenderParam(const String& name, const T& value, const T& minValue, const T& maxValue)
		{
			if (mGlobalParamDic.find(name) != mGlobalParamDic.end())
			{
				return false;
			}
			mGlobalParamDic[name].Init((void*)&value, (void*)&minValue, (void*)&maxValue, sizeof(T), JGTYPE(T));
			return true;
		}
		template<class T>
		bool SetGlobalRenderParam(const String& name, const T& data)
		{
			std::lock_guard<std::mutex> lock(mRenderParamMutex);
			if (mGlobalParamDic.find(name) == mGlobalParamDic.end())
			{
				return false;
			}

			auto& rpInfo = mGlobalParamDic[name];
			u64 dataSize = rpInfo.GetDataSize();
			if (dataSize != sizeof(T))
			{
				return false;
			}
			rpInfo.Set<T>(data);
			return true;
		}
		template<class T>
		bool GetGlobalRenderParam(const String& name, T* out_data)
		{
			std::lock_guard<std::mutex> lock(mRenderParamMutex);
			if (out_data == nullptr)
			{
				return false;
			}

			if (mGlobalParamDic.find(name) == mGlobalParamDic.end())
			{
				return false;
			}
			auto& rpInfo = mGlobalParamDic[name];
			u64 dataSize = rpInfo.GetDataSize();
			if (dataSize != sizeof(T))
			{
				return false;
			}

	
			return rpInfo.GetValue(out_data);
		}

	};
}