#pragma once
#include "JGCore.h"
#include "Application.h"



namespace JG
{
	template<class T>
	class StorableVar
	{
	private:
		String Key;
		T      Value;
		bool   mIsInit = false;


	public:
		StorableVar() = default;
		StorableVar(const String& key, const T& initData)
		{
			if (mIsInit == false)
			{
				Key = key;
				Init(key, initData);
				mIsInit = true;
			}
		}
		~StorableVar()
		{
			Save(Value);
		}
	public:
		void SetValue(const T& value)
		{
			Save(value);
		}

		const T& GetValue() const {
			return Value;
		}
	private:
		String GetDataPath() const {
			u64    hashCode  = std::hash<String>()(Key);
			String cachePath = Application::GetCachePath();
			return PathHelper::CombinePath(cachePath, std::to_string(hashCode));
		}
		void Init(const String& key, const T& initData)
		{
			String dataPath = GetDataPath();
			// 존재하면 읽기, 없으면 초기화 데이터
			if (fs::exists(dataPath) == true)
			{
				FileHelper::ReadAllByte(dataPath, (void*)&Value, sizeof(T));
			}
			else
			{
				FileHelper::WriteAllByte(dataPath, (void*)&initData, sizeof(T));
				Value = initData;
			}
		}
		void Save(const T& value)
		{
			if (mIsInit == true)
			{
				Value = value;
				FileHelper::WriteAllByte(GetDataPath(), (void*)&Value, sizeof(T));
			}
		}

	};

	template<>
	class StorableVar<String>
	{
	private:
		String Key;
		String Value;
		bool mIsInit = false;


	public:
		StorableVar() = default;
		StorableVar(const String& key, const String& initData)
		{
			if (mIsInit == false)
			{
				Init(key, initData);
				mIsInit = true;
			}
		}
		~StorableVar()
		{
			Save(Value);
		}
	public:
		void SetValue(const String& value)
		{
			Save(value);
		}

		const String& GetValue() const {
			return Value;
		}
	private:
		String GetDataPath() const {
			u64    hashCode  = std::hash<String>()(Key);
			String cachePath = Application::GetCachePath();
			return PathHelper::CombinePath(cachePath, std::to_string(hashCode));
		}
		void Init(const String& key, const String& initData)
		{
			String dataPath = GetDataPath();
			// 존재하면 읽기, 없으면 초기화 데이터
			if (fs::exists(dataPath) == true)
			{
				FileHelper::ReadAllText(dataPath, &Value);
			}
			else
			{
				FileHelper::WriteAllText(dataPath, initData);
				Value = initData;
			}
		}
		void Save(const String& value)
		{
			if (mIsInit == true)
			{
				Value = value;
				FileHelper::WriteAllText(GetDataPath(), Value);
			}
		}
	};


	using StorableString = StorableVar<String>;
	using StorableBool  = StorableVar<bool>;


	using StorableFloat = StorableVar<f32>;
	using StorableFloat2 = StorableVar<JVector2>;
	using StorableFloat3 = StorableVar<JVector3>;
	using StorableFloat4 = StorableVar<JVector4>;


	using StorableInt = StorableVar<i32>;
	using StorableInt2 = StorableVar<JVector2Int>;
	using StorableInt3 = StorableVar<JVector3Int>;
	using StorableInt4 = StorableVar<JVector4Int>;

	using StorableUint  = StorableVar<u32>;
	using StorableUint2 = StorableVar<JVector2Uint>;
	using StorableUint3 = StorableVar<JVector3Uint>;
	using StorableUint4 = StorableVar<JVector4Uint>;
}