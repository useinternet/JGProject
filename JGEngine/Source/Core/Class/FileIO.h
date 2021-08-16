#pragma once
#include <fstream>
#include "Common/Enum.h"
#include "Common/Define.h"
#include "Common/String.h"
#include "Math/JVector.h"
#include "Math/JMatrix.h"
#include "Math/JBBox.h"
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>

namespace JG
{
	class Json;
	class JsonData
	{
	private:
		bool mIsRoot = false;
		Json* mJson = nullptr;
		rapidjson::Value mValue = rapidjson::Value(rapidjson::kObjectType);
	public:
		JsonData(Json* json, bool isRoot = false) : mJson(json), mIsRoot(isRoot) {}
	public:
		SharedPtr<JsonData> CreateJsonData() const {
			return CreateSharedPtr<JsonData>(mJson);
		}
		rapidjson::Document::AllocatorType& GetJsonAllocator();
	public:
		template<class T>
		void AddMember(const String& key, const T& value);

		template<>
		void AddMember(const String& key, const SharedPtr<JsonData>& value) {
			AddMember_Base(key, value->GetValue());
		}

		void AddMember(const SharedPtr<JsonData>& value) {
			if (mValue.GetType() != rapidjson::kArrayType)
			{
				mValue.SetArray();
			}
		
			mValue.PushBack(value->mValue, GetJsonAllocator());
		}
	private:
		template<class T>
		rapidjson::Value MakeJsonValue(const T& value)
		{
			return rapidjson::Value(value);
		}
		template<>
		rapidjson::Value MakeJsonValue(const String& value);
		template<>
		rapidjson::Value MakeJsonValue(const JVector2& value);
		template<>
		rapidjson::Value MakeJsonValue(const JVector3& value);
		template<>
		rapidjson::Value MakeJsonValue(const JVector4& value);
		template<>
		rapidjson::Value MakeJsonValue(const JVector2Int& value);
		template<>
		rapidjson::Value MakeJsonValue(const JVector3Int& value);
		template<>
		rapidjson::Value MakeJsonValue(const JVector4Int& value);
		template<>
		rapidjson::Value MakeJsonValue(const JVector2Uint& value);
		template<>
		rapidjson::Value MakeJsonValue(const JVector3Uint& value);
		template<>
		rapidjson::Value MakeJsonValue(const JVector4Uint& value);
		template<>
		rapidjson::Value MakeJsonValue(const JMatrix& value);
		template<>
		rapidjson::Value MakeJsonValue(const JBBox& value);


		template<class T>
		rapidjson::Value MakeJsonValue(const List<T>& valueList);

		template<>
		rapidjson::Value MakeJsonValue(const List<jbyte>& value);
	private:
		template<class T>
		void AddMember_Base(const String& key, T& val)
		{
			rapidjson::Value keyVal;
			keyVal.SetString(key.c_str(), (rapidjson::SizeType)key.length(), mJson->GetAllocator());
			if (mIsRoot == true)
			{
				mJson->GetDocument().AddMember(keyVal, val, mJson->GetAllocator());
			}
			else
			{
				mValue.AddMember(keyVal, val, mJson->GetAllocator());
			}
		}
	public:
		u64  GetSize() const { return mValue.Size(); }
		bool GetBool() const { return mValue.GetBool(); }
		i32  GetInt32() const { return mValue.GetInt(); }
		i64  GetInt64() const { return mValue.GetInt64(); }
		u32  GetUint32() const { return mValue.GetUint(); }
		u64  GetUint64() const { return mValue.GetUint64(); }
		f32  GetFloat() const { return mValue.GetFloat(); }
		f64  GetDouble() const { return mValue.GetDouble(); }
		String GetString() const { return mValue.GetString(); }
		SharedPtr<JsonData> GetJsonDataFromIndex(i32 index) 
		{
			auto cnt = (u64)mValue.Size();
			if (cnt <= index)
			{
				return nullptr;
			}
			auto jsonData = CreateSharedPtr<JsonData>(mJson);
			jsonData->mValue = mValue[(rapidjson::SizeType)index];
			return jsonData;
		}
		String GetJsonDataKeyFromIndex(i32 index)
		{
			auto cnt = (u64)mValue.Size();
			if (cnt <= index)
			{
				return "";
			}
			return (mValue.MemberBegin() + index)->name.GetString();
		}
		List<jbyte> GetByteList() const
		{
			auto rawData = mValue.GetString();
			auto len = mValue.GetStringLength();
			List<jbyte> result;
			result.resize(len);
			memcpy(result.data(), rawData, len);
			return std::move(result);
		}
		JVector2 GetVector2() const 
		{
			JVector2 result;
			if (mValue.IsArray() == false) return result;
			i32 index = 0;
			for (auto& v : mValue.GetArray())
			{
				result[index++] = v.GetFloat();
			}
			return result;
		}
		JVector3 GetVector3() const 
		{
			JVector3 result;
			if (mValue.IsArray() == false) return result;
			i32 index = 0;
			for (auto& v : mValue.GetArray())
			{
				result[index++] = v.GetFloat();
			}
			return result;
		}
		JVector4 GetVector4() const
		{

			JVector4 result;
			i32 index = 0;
			if (mValue.IsArray() == false) return result;
			for (auto& v : mValue.GetArray())
			{
				result[index++] = v.GetFloat();
			}
			return result;
		}
		JVector2Int GetVector2Int() const {
			JVector2Int result;
			i32 index = 0;
			if (mValue.IsArray() == false) return result;
			for (auto& v : mValue.GetArray())
			{
				result[index++] = v.GetInt();
			}
			return result;
		}
		JVector3Int GetVector3Int() const {
			JVector3Int result;
			i32 index = 0;
			if (mValue.IsArray() == false) return result;
			for (auto& v : mValue.GetArray())
			{
				result[index++] = v.GetInt();
			}
			return result;
		}
		JVector4Int GetVector4Int() const {
			JVector4Int result;
			if (mValue.IsArray() == false) return result;
			i32 index = 0;
			for (auto& v : mValue.GetArray())
			{
				result[index++] = v.GetInt();
			}
			return result;
		}
		JVector2Uint GetVector2Uint() const {
			JVector2Uint result;
			if (mValue.IsArray() == false) return result;
			i32 index = 0;
			for (auto& v : mValue.GetArray())
			{
				result[index++] = v.GetInt();
			}
			return result;
		}
		JVector3Uint GetVector3Uint() const {
			JVector3Uint result;
			if (mValue.IsArray() == false) return result;
			i32 index = 0;
			for (auto& v : mValue.GetArray())
			{
				result[index++] = v.GetInt();
			}
			return result;
		}
		JVector4Uint GetVector4Uint() const {
			JVector4Uint result;
			if (mValue.IsArray() == false) return result;
			i32 index = 0;
			for (auto& v : mValue.GetArray())
			{
				result[index++] = v.GetInt();
			}
			return result;
		}

		JMatrix GetMatrix() const 
		{
			JMatrix result;
			if (mValue.IsArray() == false) return result;
			i32 col = 0; i32 row = 0;
			for (auto& v : mValue.GetArray())
			{
				result.Get(col++, row) = v.GetFloat();
				if (col >= 4)
				{
					col = 0;
					row++;
				}
			}
			return result;
		}
		JBBox GetBoundingBox() const
		{
			JBBox jbbox;
			if (mValue.IsArray() == false) return JBBox();
			i32 index = 0;
			for (auto& val : mValue.GetArray())
			{

				if (index < 3)
				{
					jbbox.min[index] = val.GetFloat();
				}
				else
				{
					jbbox.max[index - 3] = val.GetFloat();
				}


				++index;
			}
			return jbbox;
		}
	public:
		void SetString(const String& str) {
			mValue.SetString(str.c_str(), (rapidjson::SizeType)str.length(), GetJsonAllocator());
		}
		void SetBool(bool _bool) { mValue.SetBool(_bool); }
		void SetInt32(i32 _i32) { mValue.SetInt(_i32); }
		void SetInt64(i64 _i64) { mValue.SetInt64(_i64); }
		void SetUint32(u32 _u32) { mValue.SetUint(_u32); }
		void SetUint64(u64 _u64) { mValue.SetUint64(_u64); }
		void SetFloat(f32 _f32) { mValue.SetFloat(_f32); }
		void SetDouble(f64 _f64) { mValue.SetDouble(_f64); }
		void SetVector2Int(const JVector2Int& v) { mValue = MakeJsonValue(v); }
		void SetVector3Int(const JVector3Int& v) { mValue = MakeJsonValue(v); }
		void SetVector4Int(const JVector4Int& v) { mValue = MakeJsonValue(v); }
		void SetVector2Uint(const JVector2Uint& v) { mValue = MakeJsonValue(v); }
		void SetVector3Uint(const JVector3Uint& v) { mValue = MakeJsonValue(v); }
		void SetVector4Uint(const JVector4Uint& v) { mValue = MakeJsonValue(v); }

		void SetVector2(const JVector2& v) { mValue = MakeJsonValue(v); }
		void SetVector3(const JVector3& v) { mValue = MakeJsonValue(v); }
		void SetVector4(const JVector4& v) { mValue = MakeJsonValue(v); }

		void SetMatrix(const JMatrix& m) { mValue = MakeJsonValue(m); }
		void SetBoundingBox(const JBBox& jbbox) { mValue = MakeJsonValue(jbbox); }
	public:
		bool IsBool() const { return mValue.IsBool(); }
		bool IsInt32() const { return mValue.IsInt(); }
		bool IsInt64() const { return mValue.IsInt64(); }
		bool IsUint32() const { return mValue.IsUint(); }
		bool IsUint64() const { return mValue.IsUint64(); }
		bool IsFloat() const { return mValue.IsFloat(); }
		bool IsDouble() const { return mValue.IsDouble(); }
		bool IsObject() const { return mValue.IsObject(); }
		bool IsString() const { return mValue.IsString(); }
		bool IsByteList() const { return IsString(); }
		bool IsArray() const { return mValue.IsArray(); }
	public:
		SharedPtr<JsonData> GetMember(const String& key);
		
	public:
		rapidjson::Value& GetValue() {
			return mValue;
		}
	};
	class Json : public JsonData
	{
	private:
		rapidjson::Document mDoc = rapidjson::Document(rapidjson::kObjectType);

	public:
		Json() : JsonData(this, true) {}
		rapidjson::Document::AllocatorType& GetAllocator() {
			return mDoc.GetAllocator();
		}
		rapidjson::Document& GetDocument() {
			return mDoc;
		}
	public:
		static bool Write(const String& path, SharedPtr<Json> json)
		{
			auto& doc = json->GetDocument();
			
			rapidjson::StringBuffer buffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);


			if (doc.Accept(writer) == false)
			{
				return false;
			}

			std::ofstream fout(path);
			if (fout.is_open() == false)
			{
				return false;
			}
			
			fout.write(buffer.GetString(), buffer.GetSize());

			fout.close();

			return true;
		}
		static bool Read(const String& path, SharedPtr<Json> json)
		{
			auto& doc = json->GetDocument();
			

			std::ifstream fin(path);
			if (fin.is_open() == false)
			{
				return false;
			}
			u64 len = 0;
			fin.seekg(0, std::ios::end);
			len = fin.tellg();

			fin.seekg(0, std::ios::beg);

			std::string jsonStr;
			jsonStr.resize(len);
			fin.read(jsonStr.data(), len);

			fin.close();

			doc.Parse(jsonStr.c_str(), len);
			return true;
		}
		static EAssetFormat GetAssetFormat(const String& path)
		{
			EAssetFormat assetFormat = EAssetFormat::None;
			auto json = CreateSharedPtr<Json>();
			if (Json::Read(path, json) == true)
			{
				
				auto assetFormatVal = json->GetMember(JG_ASSET_FORMAT_KEY);
				if (assetFormatVal)
				{
					assetFormat = (EAssetFormat)assetFormatVal->GetUint64();
				}
			}
			return assetFormat;
		}
	};
	inline rapidjson::Document::AllocatorType& JsonData::GetJsonAllocator()
	{
		return mJson->GetAllocator();
	}
	template<class T>
	inline void JsonData::AddMember(const String& key, const T& value)
	{
		if (mJson == nullptr)
		{
			return;
		}
		auto jsonData = CreateSharedPtr<JsonData>(mJson);
	
		if constexpr (std::is_base_of<IJson, T>::value == true)
		{
			static_cast<const IJson*>(&value)->MakeJson(jsonData);
			AddMember_Base(key, jsonData->GetValue());
		}
		else
		{
			AddMember_Base(key, MakeJsonValue(value));
		}
	}


	template<>
	rapidjson::Value JsonData::MakeJsonValue(const String& value)
	{
		rapidjson::Value val;
		val.SetString(value.c_str(), (rapidjson::SizeType)value.length(), mJson->GetAllocator());
		return val;
	}
	template<>
	rapidjson::Value JsonData::MakeJsonValue(const JVector2& value)
	{
		rapidjson::Value val;
		val.SetArray();
		val.PushBack(value.x, mJson->GetAllocator());
		val.PushBack(value.y, mJson->GetAllocator());
		return val;
	}
	template<>
	inline rapidjson::Value JsonData::MakeJsonValue(const JVector3& value)
	{
		rapidjson::Value val;
		val.SetArray();
		val.PushBack(value.x, mJson->GetAllocator());
		val.PushBack(value.y, mJson->GetAllocator());
		val.PushBack(value.z, mJson->GetAllocator());

		return val;
	}
	template<>
	inline rapidjson::Value JsonData::MakeJsonValue(const JVector4& value)
	{
		rapidjson::Value val;
		val.SetArray();
		val.PushBack(value.x, mJson->GetAllocator());
		val.PushBack(value.y, mJson->GetAllocator());
		val.PushBack(value.z, mJson->GetAllocator());
		val.PushBack(value.w, mJson->GetAllocator());
		return val;
	}
	template<>
	inline rapidjson::Value JsonData::MakeJsonValue(const JVector2Int& value)
	{
		rapidjson::Value val;
		val.SetArray();
		val.PushBack(value.x, mJson->GetAllocator());
		val.PushBack(value.y, mJson->GetAllocator());
		return val;
	}
	template<>
	inline rapidjson::Value JsonData::MakeJsonValue(const JVector3Int& value)
	{
		rapidjson::Value val;
		val.SetArray();
		val.PushBack(value.x, mJson->GetAllocator());
		val.PushBack(value.y, mJson->GetAllocator());
		val.PushBack(value.z, mJson->GetAllocator());
		return val;
	}
	template<>
	inline rapidjson::Value JsonData::MakeJsonValue(const JVector4Int& value)
	{
		rapidjson::Value val;
		val.SetArray();
		val.PushBack(value.x, mJson->GetAllocator());
		val.PushBack(value.y, mJson->GetAllocator());
		val.PushBack(value.z, mJson->GetAllocator());
		val.PushBack(value.w, mJson->GetAllocator());
		return val;
	}
	template<>
	inline rapidjson::Value JsonData::MakeJsonValue(const JVector2Uint& value)
	{
		rapidjson::Value val;
		val.SetArray();
		val.PushBack(value.x, mJson->GetAllocator());
		val.PushBack(value.y, mJson->GetAllocator());
		return val;
	}
	template<>
	inline rapidjson::Value JsonData::MakeJsonValue(const JVector3Uint& value)
	{
		rapidjson::Value val;
		val.SetArray();
		val.PushBack(value.x, mJson->GetAllocator());
		val.PushBack(value.y, mJson->GetAllocator());
		val.PushBack(value.z, mJson->GetAllocator());
		return val;
	}
	template<>
	inline rapidjson::Value JsonData::MakeJsonValue(const JVector4Uint& value)
	{
		rapidjson::Value val;
		val.SetArray();
		val.PushBack(value.x, mJson->GetAllocator());
		val.PushBack(value.y, mJson->GetAllocator());
		val.PushBack(value.z, mJson->GetAllocator());
		val.PushBack(value.w, mJson->GetAllocator());
		return val;
	}
	template<>
	inline rapidjson::Value JsonData::MakeJsonValue(const JMatrix& value)
	{
		rapidjson::Value val;
		val.SetArray();
		for (i32 r = 0; r < 4; ++r)
		{
			for (i32 c = 0; c < 4; ++c)
			{
				f32 f = value.Get_C(c, r);
				val.PushBack(f, mJson->GetAllocator());
			}
		}
		return val;
	}
	template<>
	inline rapidjson::Value JsonData::MakeJsonValue(const JBBox& value)
	{
		rapidjson::Value val;
		val.SetArray();
		val.PushBack(value.min.x, mJson->GetAllocator());
		val.PushBack(value.min.y, mJson->GetAllocator());
		val.PushBack(value.min.z, mJson->GetAllocator());
		val.PushBack(value.max.x, mJson->GetAllocator());
		val.PushBack(value.max.y, mJson->GetAllocator());
		val.PushBack(value.max.z, mJson->GetAllocator());
		return val;
	}
	template<class T>
	rapidjson::Value JsonData::MakeJsonValue(const List<T>& valueList)
	{
		rapidjson::Value valArr; valArr.SetArray();
		for (auto& value : valueList)
		{
			rapidjson::Value val;
			if constexpr (std::is_base_of<IJson, T>::value == true)
			{
				auto jsonData = CreateSharedPtr<JsonData>(mJson);
				static_cast<const IJson*>(&value)->MakeJson(jsonData);
				val = jsonData->GetValue();
			}
			else
			{
				val = MakeJsonValue(value);
			}
			valArr.PushBack(val, mJson->GetAllocator());
		}
		return valArr;
	}
	template<>
	rapidjson::Value JsonData::MakeJsonValue(const List<jbyte>& value)
	{
		rapidjson::Value val;
		val.SetString(value.data(), (rapidjson::SizeType)value.size(), mJson->GetAllocator());
		return val;
	}
	inline SharedPtr<JsonData> JsonData::GetMember(const String& key)
	{
		bool isFind = (mIsRoot) ?
			mJson->GetDocument().FindMember(key.c_str()) != mJson->GetDocument().MemberEnd() :
			mValue.FindMember(key.c_str()) != mValue.MemberEnd();

		if (isFind == false)
		{
			return nullptr;
		}
		auto jsonData = CreateSharedPtr<JsonData>(mJson);
		jsonData->mValue = (mIsRoot) ? mJson->GetDocument()[key.c_str()] : mValue[key.c_str()];
		return jsonData;
	}
}