#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "String/String.h"
#include "String/Name.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"

class PJson;
class PJsonData;

class IJsonable
{
public:
	virtual ~IJsonable() = default;
protected:
	virtual void WriteJson(PJsonData& json) {}
	virtual void ReadJson(PJsonData& json)  {}
};

class PJsonData : public IMemoryObject
{
	friend class PJson;

	PJson* _pOwnerJson = nullptr;

	rapidjson::Value _value;
	bool _bIsRoot;
private:
	PJsonData(PJson* ownerJson, bool bIsRoot = false);
	virtual ~PJsonData() = default;

public:
	template<class T>
	void AddMember(const PString& key, const T& data);

	template<class T>
	void AddMember(const T& data);

private:
	void addMemberInternal(const PString& key, rapidjson::Value& value);

	template<class T>
	rapidjson::Value MakeJsonValue(const T& value);
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
};

template<class T>
void PJsonData::AddMember(const PString& key, const T& value)
{
	if constexpr (std::is_base_of<IJsonable, T>::value == true)
	{
		PJsonData jsonData = PJsonData(_pOwnerJson);

		static_cast<const IJsonable*>(&value)->WriteJson(jsonData);

		addMemberInternal(key, jsonData._value);
	}
	else
	{
		addMemberInternal(key, MakeJsonValue(value));
	}
}

template<class T>
void PJsonData::AddMember(const T& data)
{

}

template<class T>
rapidjson::Value PJsonData::MakeJsonValue(const T& value)
{
	return rapidjson::Value(value);
}