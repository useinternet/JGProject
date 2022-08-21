#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "String/String.h"
#include "String/Name.h"
#include "Misc/Log.h"

#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "Math/BBox.h"
#include "Math/Frustum.h"
#include "Math/Plane.h"
#include "Math/Color.h"
#include "Math/Rect.h"
#include "Math/Matrix.h"
#include "Math/Ray.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"

class PJson;
class PJsonData;

class IJsonable
{
	friend PJsonData;
public:
	virtual ~IJsonable() = default;
protected:
	virtual void WriteJson(PJsonData& json) const {}
	virtual void ReadJson(PJsonData& json) {}
};

class PJsonData : public IMemoryObject, public IJsonable
{
	friend class PJson;

	PJson* _pOwnerJson = nullptr;
	bool   _bIsRoot    = false;

	mutable rapidjson::Value _value;
public:
	PJsonData() = default;
	virtual ~PJsonData() = default;

private:
	PJsonData(PJson* ownerJson, bool bIsRoot = false);

public:
	template<class T>
	void AddMember(const PString& key, const T& data);

	template<class T>
	void AddMember(const T& data);

	bool FindMember(const PString& key, PJsonData* outJsonData)   const;
	bool FindMemberFromIndex(int32 index, PJsonData* outJsonData) const;

	template<class T>
	bool GetData(const PString& key, T* outData) const;

	template<class T>
	bool  GetData(T* outData) const;

	int32 GetSize() const;
	bool  IsValid() const;

protected:
	virtual void WriteJson(PJsonData& json) const override;
	virtual void ReadJson(PJsonData& json) override;

private:
#pragma region Make Json Value Declare
	void addMemberInternal(const PString& key, rapidjson::Value& value);

	template<class T>
	void pushValueInJsonValue(rapidjson::Value& outJsonValue, const T& value);

	template<class T>
	void setValueInJsonValue(rapidjson::Value& outJsonValue, const T& value);

	template<class T>
	rapidjson::Value makeJsonValue(const T& value);

	template<>
	rapidjson::Value makeJsonValue(const PString& value);

	template<>
	rapidjson::Value makeJsonValue(const PName& value);

	template<>
	rapidjson::Value makeJsonValue(const PVector2& value);

	template<>
	rapidjson::Value makeJsonValue(const PVector3& value);

	template<>
	rapidjson::Value makeJsonValue(const PVector4& value);

	template<>
	rapidjson::Value makeJsonValue(const PVector2Int& value);

	template<>
	rapidjson::Value makeJsonValue(const PVector3Int& value);

	template<>
	rapidjson::Value makeJsonValue(const PVector4Int& value);

	template<>
	rapidjson::Value makeJsonValue(const PVector2Uint& value);

	template<>
	rapidjson::Value makeJsonValue(const PVector3Uint& value);

	template<>
	rapidjson::Value makeJsonValue(const PVector4Uint& value);

	template<>
	rapidjson::Value makeJsonValue(const PMatrix& value);

	template<>
	rapidjson::Value makeJsonValue(const PQuaternion& value);

	template<>
	rapidjson::Value makeJsonValue(const PRect& value);

	template<>
	rapidjson::Value makeJsonValue(const PRay& value);

	template<>
	rapidjson::Value makeJsonValue(const PFrustum& value);

	template<>
	rapidjson::Value makeJsonValue(const PBBox& value);

	template<>
	rapidjson::Value makeJsonValue(const PPlane& value);

	template<class T>
	rapidjson::Value makeJsonValue(const PList<T>& values);

	template<class T>
	rapidjson::Value makeJsonValue(const PStack<T>& values);

	template<class T>
	rapidjson::Value makeJsonValue(const PQueue<T>& values);

	template<class KeyType, class ValueType>
	rapidjson::Value makeJsonValue(const PHashMap<KeyType, ValueType>& values);

	template<class KeyType, class ValueType>
	rapidjson::Value makeJsonValue(const PMap<KeyType, ValueType>& values);

	template<class KeyType>
	rapidjson::Value makeJsonValue(const PHashSet<KeyType>& values);

	template<class KeyType>
	rapidjson::Value makeJsonValue(const PSet<KeyType>& values);
#pragma endregion
#pragma region Read Json Value Declare
	template<class T>
	bool getValueInJsonValue(rapidjson::Value& jsonValue, T* value) const;

	template<class T>
	bool readJsonValue(T* value) const;

	template<>
	bool readJsonValue(bool* value) const;

	template<>
	bool readJsonValue(int32* value) const;

	template<>
	bool readJsonValue(int64* value) const;

	template<>
	bool readJsonValue(uint32* value) const;

	template<>
	bool readJsonValue(uint64* value) const;

	template<>
	bool readJsonValue(float32* value) const;

	template<>
	bool readJsonValue(float64* value) const;

	template<>
	bool readJsonValue(PString* value) const;

	template<>
	bool readJsonValue(PName* value) const;

	template<>
	bool readJsonValue(PVector2* value) const;

	template<>
	bool readJsonValue(PVector3* value) const;

	template<>
	bool readJsonValue(PVector4* value) const;

	template<>
	bool readJsonValue(PVector2Int* value) const;

	template<>
	bool readJsonValue(PVector3Int* value) const;

	template<>
	bool readJsonValue(PVector4Int* value) const;

	template<>
	bool readJsonValue(PVector2Uint* value) const;

	template<>
	bool readJsonValue(PVector3Uint* value) const;

	template<>
	bool readJsonValue(PVector4Uint* value) const;

	template<>
	bool readJsonValue(PMatrix* value) const;

	template<>
	bool readJsonValue(PQuaternion* value) const;

	template<>
	bool readJsonValue(PRect* value) const;

	template<>
	bool readJsonValue(PRay* value) const;

	template<>
	bool readJsonValue(PFrustum* value) const;

	template<>
	bool readJsonValue(PBBox* value) const;

	template<>
	bool readJsonValue(PPlane* value) const;

	template<class T>
	bool readJsonValue(PList<T>* values) const;

	template<class T>
	bool readJsonValue(PStack<T>* values) const;

	template<class T>
	bool readJsonValue(PQueue<T>* values) const;

	template<class KeyType, class ValueType>
	bool readJsonValue(PHashMap<KeyType, ValueType>* values) const;

	template<class KeyType, class ValueType>
	bool readJsonValue(PMap<KeyType, ValueType>* values) const;

	template<class KeyType>
	bool readJsonValue(PHashSet<KeyType>* values) const;

	template<class KeyType>
	bool readJsonValue(PSet<KeyType>* values) const;

#pragma endregion
#pragma region Util Function Declare
	template<class VectorType>
	rapidjson::Value makeJsonValue_Vector(const VectorType& value, int32 numVectorEelement);

	template<class VectorType>
	bool readJsonValue_Vector(VectorType* value, int32 numVectorEelement) const;
#pragma endregion
};

class PJson : public PJsonData
{
	rapidjson::Document _doc = rapidjson::Document(rapidjson::kObjectType);
public:
	PJson();
	virtual ~PJson() = default;

public:
	rapidjson::Document::AllocatorType& GetAllocator();
	rapidjson::Document& GetDocument();

public:
	static bool ToString(PJson& json, PString* outStr);
	static bool ToObject(const PString& jsonText, PJson* outJson);
};

template<class T>
inline void PJsonData::AddMember(const PString& key, const T& value)
{
	if (_pOwnerJson == nullptr)
	{
		return;
	}

	if constexpr (std::is_base_of<IJsonable, T>::value == true)
	{
		PJsonData jsonData = PJsonData(_pOwnerJson);

		static_cast<const IJsonable*>(&value)->WriteJson(jsonData);

		addMemberInternal(key, jsonData._value);
	}
	else
	{
		rapidjson::Value val = makeJsonValue(value);
		addMemberInternal(key, val);
	}
}

template<class T>
inline void PJsonData::AddMember(const T& value)
{
	pushValueInJsonValue(_value, value);
}

template<class T>
bool PJsonData::GetData(const PString& key, T* outData) const
{
	if (IsValid() == false || outData == nullptr)
	{
		return false;
	}

	PJsonData jsonData;
	if (FindMember(key, &jsonData) == false)
	{
		return false;
	}

	return jsonData.GetData(outData);
}

template<class T>
inline bool PJsonData::GetData(T* outData) const
{
	if (IsValid() == false || outData == nullptr)
	{
		return false;
	}

	return getValueInJsonValue(_value, outData);
}

#pragma region Make Json Value Impl
template<class T>
inline void PJsonData::pushValueInJsonValue(rapidjson::Value& outJsonValue, const T& value)
{
	if (_pOwnerJson == nullptr)
	{
		return;
	}

	if (outJsonValue.GetType() != rapidjson::kArrayType)
	{
		outJsonValue.SetArray();
	}

	if constexpr (std::is_base_of<IJsonable, T>::value == true)
	{
		PJsonData jsonData = PJsonData(_pOwnerJson);
		static_cast<const IJsonable*>(&value)->WriteJson(jsonData);
		outJsonValue.PushBack(jsonData._value, _pOwnerJson->GetAllocator());
	}
	else
	{
		rapidjson::Value val = makeJsonValue(value);
		outJsonValue.PushBack(val, _pOwnerJson->GetAllocator());
	}
}

template<class T>
inline void PJsonData::setValueInJsonValue(rapidjson::Value& outJsonValue, const T& value)
{
	if (_pOwnerJson == nullptr)
	{
		return;
	}

	if (outJsonValue.GetType() != rapidjson::kObjectType)
	{
		outJsonValue.SetObject();
	}

	if constexpr (std::is_base_of<IJsonable, T>::value == true)
	{
		PJsonData jsonData = PJsonData(_pOwnerJson);
		static_cast<const IJsonable*>(&value)->WriteJson(jsonData);

		outJsonValue = jsonData._value;
	}
	else
	{
		outJsonValue = makeJsonValue(value);
	}
}

template<class T>
inline rapidjson::Value PJsonData::makeJsonValue(const T& value)
{
	return rapidjson::Value(value);
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PString& value)
{
	rapidjson::Value val;

	PString str;
	value.SubString(str, 0, std::strlen(value.GetCStr()));

	val.SetString(str.GetCStr(), (rapidjson::SizeType)str.Length(), _pOwnerJson->GetAllocator());

	return val;
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PName& value)
{
	rapidjson::Value val;
	PString str;
	value.ToString(str);

	return makeJsonValue<PString>(str);
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PVector2& value)
{
	return makeJsonValue_Vector(value, 2);
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PVector3& value)
{
	return makeJsonValue_Vector(value, 3);
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PVector4& value)
{
	return makeJsonValue_Vector(value, 4);
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PVector2Int& value)
{
	return makeJsonValue_Vector(value, 2);
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PVector3Int& value)
{
	return makeJsonValue_Vector(value, 3);
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PVector4Int& value)
{
	return makeJsonValue_Vector(value, 4);
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PVector2Uint& value)
{
	return makeJsonValue_Vector(value, 2);
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PVector3Uint& value)
{
	return makeJsonValue_Vector(value, 3);
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PVector4Uint& value)
{
	return makeJsonValue_Vector(value, 4);
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PMatrix& value)
{
	rapidjson::Value val;
	val.SetArray();

	for (int32 i = 0; i < 4; ++i)
	{
		for (int32 j = 0; j < 4; ++j)
		{
			val.PushBack(value.Get_C(i, j), _pOwnerJson->GetAllocator());
		}
	}
	
	return val;
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PQuaternion& value)
{
	return makeJsonValue_Vector(value, 4);
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PRect& value)
{
	rapidjson::Value val;
	val.SetArray();

	val.PushBack(value.left, _pOwnerJson->GetAllocator());
	val.PushBack(value.top, _pOwnerJson->GetAllocator());
	val.PushBack(value.right, _pOwnerJson->GetAllocator());
	val.PushBack(value.bottom, _pOwnerJson->GetAllocator());

	return val;
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PRay& value)
{
	rapidjson::Value val;
	val.SetArray();

	val.PushBack(value.origin.x, _pOwnerJson->GetAllocator());
	val.PushBack(value.origin.y, _pOwnerJson->GetAllocator());
	val.PushBack(value.origin.z, _pOwnerJson->GetAllocator());

	val.PushBack(value.dir.x, _pOwnerJson->GetAllocator());
	val.PushBack(value.dir.y, _pOwnerJson->GetAllocator());
	val.PushBack(value.dir.z, _pOwnerJson->GetAllocator());

	return val;
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PFrustum& value)
{
	rapidjson::Value val;
	val.SetArray();

	for (int32 i = 0; i < PFrustum::Count; ++i)
	{
		val.PushBack(value.Plane[i].N.x, _pOwnerJson->GetAllocator());
		val.PushBack(value.Plane[i].N.y, _pOwnerJson->GetAllocator());
		val.PushBack(value.Plane[i].N.z, _pOwnerJson->GetAllocator());

		val.PushBack(value.Plane[i].D, _pOwnerJson->GetAllocator());
	}

	return val;
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PBBox& value)
{
	rapidjson::Value val;
	val.SetArray();

	val.PushBack(value.min.x, _pOwnerJson->GetAllocator());
	val.PushBack(value.min.y, _pOwnerJson->GetAllocator());
	val.PushBack(value.min.z, _pOwnerJson->GetAllocator());

	val.PushBack(value.max.x, _pOwnerJson->GetAllocator());
	val.PushBack(value.max.y, _pOwnerJson->GetAllocator());
	val.PushBack(value.max.z, _pOwnerJson->GetAllocator());

	return val;
}

template<>
inline rapidjson::Value PJsonData::makeJsonValue(const PPlane& value)
{
	rapidjson::Value val;
	val.SetArray();

	val.PushBack(value.N.x, _pOwnerJson->GetAllocator());
	val.PushBack(value.N.y, _pOwnerJson->GetAllocator());
	val.PushBack(value.N.z, _pOwnerJson->GetAllocator());

	val.PushBack(value.D, _pOwnerJson->GetAllocator());

	return val;
}

template<class T>
inline rapidjson::Value PJsonData::makeJsonValue(const PList<T>& values)
{
	rapidjson::Value valArr; valArr.SetArray();

	for (const T& value : values)
	{
		pushValueInJsonValue(valArr, value);
	}

	return valArr;
}

template<class T>
inline rapidjson::Value PJsonData::makeJsonValue(const PStack<T>& values)
{
	rapidjson::Value valArr; valArr.SetArray();

	PStack<T> tempStack = values;

	while (tempStack.empty() == false)
	{
		T value = tempStack.top(); tempStack.pop();
		pushValueInJsonValue(valArr, value);
	}

	return valArr;
}

template<class T>
inline rapidjson::Value PJsonData::makeJsonValue(const PQueue<T>& values)
{
	rapidjson::Value valArr; valArr.SetArray();

	PQueue<T> tempQueue = values;

	while (tempQueue.empty() == false)
	{
		T value = tempQueue.front(); tempQueue.pop();
		pushValueInJsonValue(valArr, value);
	}

	return valArr;
}

template<class KeyType, class ValueType>
inline rapidjson::Value PJsonData::makeJsonValue(const PHashMap<KeyType, ValueType>& values)
{
	rapidjson::Value valArr; 
	valArr.SetArray();

	for (const PPair<KeyType, ValueType>& pair : values)
	{
		rapidjson::Value pairVal;
		pairVal.SetArray();

		rapidjson::Value keyVal;
		rapidjson::Value valueVal;

		setValueInJsonValue(keyVal, pair.first);
		setValueInJsonValue(valueVal, pair.second);

		pairVal.PushBack(keyVal, _pOwnerJson->GetAllocator());
		pairVal.PushBack(valueVal, _pOwnerJson->GetAllocator());

		valArr.PushBack(pairVal, _pOwnerJson->GetAllocator());
	}

	return valArr;
}

template<class KeyType, class ValueType>
inline rapidjson::Value PJsonData::makeJsonValue(const PMap<KeyType, ValueType>& values)
{
	rapidjson::Value valArr; 
	valArr.SetArray();

	for (const PPair<KeyType, ValueType>& pair : values)
	{
		rapidjson::Value pairVal;
		pairVal.SetArray();

		rapidjson::Value keyVal;
		rapidjson::Value valueVal;

		setValueInJsonValue(keyVal, pair.first);
		setValueInJsonValue(valueVal, pair.second);

		pairVal.PushBack(keyVal, _pOwnerJson->GetAllocator());
		pairVal.PushBack(valueVal, _pOwnerJson->GetAllocator());

		valArr.PushBack(pairVal, _pOwnerJson->GetAllocator());
	}

	return valArr;
}

template<class KeyType>
inline rapidjson::Value PJsonData::makeJsonValue(const PHashSet<KeyType>& values)
{
	rapidjson::Value valArr; valArr.SetArray();

	for (const KeyType& key : values)
	{
		pushValueInJsonValue(valArr, key);
	}

	return valArr;
}

template<class KeyType>
inline rapidjson::Value PJsonData::makeJsonValue(const PSet<KeyType>& values)
{
	rapidjson::Value valArr; valArr.SetArray();

	for (const KeyType& key : values)
	{
		pushValueInJsonValue(valArr, key);
	}

	return valArr;
}
#pragma endregion
#pragma region Read Json Value Impl
template<class T>
inline bool PJsonData::getValueInJsonValue(rapidjson::Value& jsonValue, T* value) const
{
	if (value == nullptr)
	{
		return false;
	}

	PJsonData jsonData = PJsonData(_pOwnerJson);
	jsonData._value = jsonValue;

	if constexpr (std::is_base_of<IJsonable, T>::value == true)
	{
		static_cast<IJsonable*>(value)->ReadJson(jsonData);

		return true;
	}
	else
	{
		return jsonData.readJsonValue(value);
	}
}

template<class T>
inline bool PJsonData::readJsonValue(T* value) const
{
	JG_LOG_ERROR("{0} is not supported read json", typeid(T).name());
	return false;
}

template<>
inline bool PJsonData::readJsonValue(bool* value) const
{
	if (_value.IsBool() == false || value == nullptr)
	{
		return false;
	}

	*value = _value.GetBool();

	return true;
}

template<>
inline bool PJsonData::readJsonValue(int32* value) const
{
	if (_value.IsInt() == false || value == nullptr)
	{
		return false;
	}
	
	*value = _value.GetInt();

	return true;
}

template<>
inline bool PJsonData::readJsonValue(int64* value) const
{
	if (_value.IsInt64() == false || value == nullptr)
	{
		return false;
	}

	*value = _value.GetInt64();

	return true;
}

template<>
inline bool PJsonData::readJsonValue(uint32* value) const
{
	if (_value.IsUint() == false || value == nullptr)
	{
		return false;
	}

	*value = _value.GetUint();

	return true;
}

template<>
inline bool PJsonData::readJsonValue(uint64* value) const
{
	if (_value.IsUint64() == false || value == nullptr)
	{
		return false;
	}

	*value = _value.GetUint64();

	return true;
}

template<>
inline bool PJsonData::readJsonValue(float32* value) const
{
	if (_value.IsFloat() == false || value == nullptr)
	{
		return false;
	}

	*value = _value.GetFloat();

	return true;
}

template<>
inline bool PJsonData::readJsonValue(float64* value) const
{
	if (_value.IsDouble() == false || value == nullptr)
	{
		return false;
	}

	*value = _value.GetDouble();

	return true;
}

template<>
inline bool PJsonData::readJsonValue(PString* value) const
{
	if (_value.IsString() == false || value == nullptr)
	{
		return false;
	}

	*value = _value.GetString();

	return true;
}

template<>
inline bool PJsonData::readJsonValue(PName* value) const
{
	if (_value.IsString() == false || value == nullptr)
	{
		return false;
	}

	*value = _value.GetString();

	return true;
}

template<>
inline bool PJsonData::readJsonValue(PVector2* value) const
{
	return readJsonValue_Vector(value, 2);
}

template<>
inline bool PJsonData::readJsonValue(PVector3* value) const
{
	return readJsonValue_Vector(value, 3);
}

template<>
inline bool PJsonData::readJsonValue(PVector4* value) const
{
	return readJsonValue_Vector(value, 4);
}

template<>
inline bool PJsonData::readJsonValue(PVector2Int* value) const
{
	return readJsonValue_Vector(value, 2);
}

template<>
inline bool PJsonData::readJsonValue(PVector3Int* value) const
{
	return readJsonValue_Vector(value, 3);
}

template<>
inline bool PJsonData::readJsonValue(PVector4Int* value) const
{
	return readJsonValue_Vector(value, 4);
}

template<>
inline bool PJsonData::readJsonValue(PVector2Uint* value) const
{
	return readJsonValue_Vector(value, 2);
}

template<>
inline bool PJsonData::readJsonValue(PVector3Uint* value) const
{
	return readJsonValue_Vector(value, 3);
}

template<>
inline bool PJsonData::readJsonValue(PVector4Uint* value) const
{
	return readJsonValue_Vector(value, 4);
}

template<>
inline bool PJsonData::readJsonValue(PMatrix* value) const
{
	if (_value.IsArray() == false || value == nullptr)
	{
		return false;
	}

	*value = PMatrix();

	int32 index = 0;
	for (int32 i = 0; i < 4; ++i)
	{
		for (int32 j = 0; j < 4; ++j)
		{
			PJsonData jsonData = PJsonData(_pOwnerJson);

			if (FindMemberFromIndex(index, &jsonData) == false)
			{
				return false;
			}

			if (jsonData.GetData(&(value->Get(i, j))) == false)
			{
				return false;
			}

			++index;
		}
	}

	return true;
}

template<>
inline bool PJsonData::readJsonValue(PQuaternion* value) const
{
	return readJsonValue_Vector(value, 4);
}

template<>
inline bool PJsonData::readJsonValue(PRect* value) const
{
	if (_value.IsArray() == false || value == nullptr)
	{
		return false;
	}

	PJsonData jsonData;
	if (FindMemberFromIndex(0, &jsonData) == false || jsonData.GetData(&value->left) == false)
	{
		return false;
	}

	if (FindMemberFromIndex(1, &jsonData) == false || jsonData.GetData(&value->top) == false)
	{
		return false;
	}

	if (FindMemberFromIndex(2, &jsonData) == false || jsonData.GetData(&value->right) == false)
	{
		return false;
	}

	if (FindMemberFromIndex(3, &jsonData) == false || jsonData.GetData(&value->bottom) == false)
	{
		return false;
	}

	return true;
}

template<>
inline bool PJsonData::readJsonValue(PRay* value) const
{
	if (_value.IsArray() == false || value == nullptr)
	{
		return false;
	}

	*value = PRay();

	PJsonData jsonData;
	int32 index = 0;

	for (int32 i = 0; i < 3; ++i)
	{
	
		if (FindMemberFromIndex(index, &jsonData) == false || jsonData.GetData(&(value->origin)[i]) == false)
		{
			return false;
		}

		++index;
	}

	for (int32 i = 0; i < 3; ++i)
	{
		PJsonData jsonData;
		if (FindMemberFromIndex(index, &jsonData) == false || jsonData.GetData(&(value->dir)[i]) == false)
		{
			return false;
		}

		++index;
	}

	return true;
}

template<>
inline bool PJsonData::readJsonValue(PFrustum* value) const
{
	if (_value.IsArray() == false || value == nullptr)
	{
		return false;
	}

	*value = PFrustum();

	PJsonData jsonData;
	int32	  index = 0;

	for (int32 i = 0; i < PFrustum::Count; ++i)
	{
	
		for (int32 j = 0; j < 3; ++j)
		{
			if (FindMemberFromIndex(index, &jsonData) == false || jsonData.GetData(&(value->Plane[i].N[j])) == false)
			{
				return false;
			}

			++index;
		}

		if (FindMemberFromIndex(index, &jsonData) == false || jsonData.GetData(&(value->Plane[i].D)) == false)
		{
			return false;
		}
	}

	return true;
}

template<>
inline bool PJsonData::readJsonValue(PBBox* value) const
{
	if (_value.IsArray() == false || value == nullptr)
	{
		return false;
	}

	*value = PBBox();

	PJsonData jsonData;
	int32 index = 0;

	for (int32 i = 0; i < 3; ++i)
	{

		if (FindMemberFromIndex(index, &jsonData) == false || jsonData.GetData(&(value->min)[i]) == false)
		{
			return false;
		}

		++index;
	}

	for (int32 i = 0; i < 3; ++i)
	{
		PJsonData jsonData;
		if (FindMemberFromIndex(index, &jsonData) == false || jsonData.GetData(&(value->max)[i]) == false)
		{
			return false;
		}

		++index;
	}

	return true;
}

template<>
inline bool PJsonData::readJsonValue(PPlane* value) const
{
	if (_value.IsArray() == false || value == nullptr)
	{
		return false;
	}

	*value = PPlane();

	PJsonData jsonData;
	int32	  index = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (FindMemberFromIndex(index, &jsonData) == false || jsonData.GetData(&(value->N[i])) == false)
		{
			return false;
		}

		++index;
	}

	if (FindMemberFromIndex(index, &jsonData) == false || jsonData.GetData(&(value->D)) == false)
	{
		return false;
	}

	return true;
}

template<class T>
inline bool PJsonData::readJsonValue(PList<T>* values) const
{
	if (_value.IsArray() == false)
	{
		return false;
	}
	bool  bResult = true;
	int32 len     = GetSize();

	(*values).resize(len);

	for (int32 i = 0; i < len; ++i)
	{
		PJsonData jsonData;
		if (FindMemberFromIndex(i, &jsonData) == false)
		{
			bResult = false;
			continue;
		}

		if (getValueInJsonValue(jsonData._value, &(*values)[i]) == false)
		{
			bResult = false;
		}
	}

	return bResult;
}

template<class T>
inline bool PJsonData::readJsonValue(PStack<T>* values) const
{
	if (_value.IsArray() == false)
	{
		return false;
	}

	(*values) = PStack<T>();
	
	bool  bResult = true;
	int32 len = GetSize();

	for (int32 i = len - 1; i >= 0; --i)
	{
		PJsonData jsonData;
		if (FindMemberFromIndex(i, &jsonData) == false)
		{
			bResult = false;
			continue;
		}

		T data;
		if (getValueInJsonValue(jsonData._value, &data) == false)
		{
			bResult = false;
			continue;
		}

		(*values).push(data);
	}

	return bResult;
}

template<class T>
inline bool PJsonData::readJsonValue(PQueue<T>* values) const
{
	if (_value.IsArray() == false)
	{
		return false;
	}

	(*values) = PQueue<T>();

	bool  bResult = true;
	int32 len = GetSize();

	for (int32 i = 0; i < len; ++i)
	{
		PJsonData jsonData;
		if (FindMemberFromIndex(i, &jsonData) == false)
		{
			bResult = false;
			continue;
		}

		T data;
		if (getValueInJsonValue(jsonData._value, &data) == false)
		{
			bResult = false;
			continue;
		}

		(*values).push(data);
	}

	return bResult;
}

template<class KeyType, class ValueType>
inline bool PJsonData::readJsonValue(PHashMap<KeyType, ValueType>* values) const
{
	if (_value.IsArray() == false)
	{
		return false;
	}

	(*values).clear();

	bool  bResult = true;
	int32 len = GetSize();

	for (int32 i = 0; i < len; ++i)
	{
		PJsonData jsonData;
		if (FindMemberFromIndex(i, &jsonData) == false)
		{
			bResult = false;
			continue;
		}

		PJsonData keyJson;
		if (jsonData.FindMemberFromIndex(0, &keyJson) == false)
		{
			bResult = false;
			continue;
		}

		KeyType keyData;
		if (getValueInJsonValue(keyJson._value, &keyData) == false)
		{
			bResult = false;
			continue;
		}

		PJsonData valJson;
		if (jsonData.FindMemberFromIndex(1, &valJson) == false)
		{
			bResult = false;
			continue;
		}
		
		ValueType valData;
		if (getValueInJsonValue(valJson._value, &valData) == false)
		{
			bResult = false;
			continue;
		}

		(*values).emplace(keyData, valData);
	}

	return bResult;
}

template<class KeyType, class ValueType>
inline bool PJsonData::readJsonValue(PMap<KeyType, ValueType>* values) const
{
	if (_value.IsArray() == false)
	{
		return false;
	}

	(*values).clear();

	bool  bResult = true;
	int32 len = GetSize();

	for (int32 i = 0; i < len; ++i)
	{
		PJsonData jsonData;
		if (FindMemberFromIndex(i, &jsonData) == false)
		{
			bResult = false;
			continue;
		}

		PJsonData keyJson;
		if (jsonData.FindMemberFromIndex(0, &keyJson) == false)
		{
			bResult = false;
			continue;
		}

		KeyType   keyData;
		if (getValueInJsonValue(keyJson._value, &keyData) == false)
		{
			bResult = false;
			continue;
		}

		PJsonData valJson;
		if (jsonData.FindMemberFromIndex(1, &valJson) == false)
		{
			bResult = false;
			continue;
		}

		ValueType valData;
		if (getValueInJsonValue(valJson._value, &valData) == false)
		{
			bResult = false;
			continue;
		}

		(*values).emplace(keyData, valData);
	}

	return bResult;
}

template<class KeyType>
inline bool PJsonData::readJsonValue(PHashSet<KeyType>* values) const
{
	if (_value.IsArray() == false)
	{
		return false;
	}

	(*values).clear();

	bool  bResult = true;
	int32 len = GetSize();

	for (int32 i = 0; i < len; ++i)
	{
		PJsonData jsonData;
		if (FindMemberFromIndex(i, &jsonData) == false)
		{
			bResult = false;
			continue;
		}

		KeyType keyData;
		if (getValueInJsonValue(jsonData._value, &keyData) == false)
		{
			bResult = false;
			continue;
		}

		(*values).insert(keyData);
	}

	return bResult;
}

template<class KeyType>
inline bool PJsonData::readJsonValue(PSet<KeyType>* values) const
{
	if (_value.IsArray() == false)
	{
		return false;
	}

	(*values).clear();

	bool  bResult = true;
	int32 len = GetSize();

	for (int32 i = 0; i < len; ++i)
	{
		PJsonData jsonData;
		if (FindMemberFromIndex(i, &jsonData) == false)
		{
			bResult = false;
			continue;
		}

		KeyType keyData;
		if (getValueInJsonValue(jsonData._value, &keyData) == false)
		{
			bResult = false;
			continue;
		}

		(*values).insert(keyData);
	}

	return bResult;
}

#pragma endregion
#pragma region Util Function Impl
template<class VectorType>
inline rapidjson::Value PJsonData::makeJsonValue_Vector(const VectorType& value, int32 numVectorEelement)
{
	VectorType inValue = value;
	rapidjson::Value val;
	val.SetArray();

	for (int32 i = 0; i < numVectorEelement; ++i)
	{
		val.PushBack(inValue[i], _pOwnerJson->GetAllocator());
	}

	return val;
}

template<class VectorType>
inline bool PJsonData::readJsonValue_Vector(VectorType* value, int32 numVectorEelement) const
{
	if (_value.IsArray() == false || value == nullptr)
	{
		return false;
	}

	*value = VectorType();

	for (int32 i = 0; i < numVectorEelement; ++i)
	{
		PJsonData jsonData = PJsonData(_pOwnerJson);
		if (FindMemberFromIndex(i, &jsonData) == false)
		{
			return false;
		}

		if (jsonData.GetData(&(*value)[i]) == false)
		{
			return false;
		}
	}

	return true;
}
#pragma endregion
