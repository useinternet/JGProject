#include "PCH/PCH.h"
#include "JGObject.h"
#include "ObjectGlobals.h"

JGType JGObject::GetType() const
{
	return JGTYPE(JGObject);
}

void JGObject::SetName(const PName& name)
{
	_name = name;
}

const PName& JGObject::GetName() const
{
	if (_name == NAME_NONE)
	{
		_name = GetType().GetName();
	}

	return _name;
}

void JGObject::WriteJson(PJsonData& json) const
{
	PJsonData objJson = json.CreateJsonData();

	objJson.AddMember("Name", GetName());

	json.AddMember("JGObject", objJson);

	OnWriteJson(json);
}

void JGObject::ReadJson(const PJsonData& json)
{
	PJsonData objJson;
	if (json.FindMember("JGObject", &objJson) == false)
	{
		JG_LOG(Json, ELogLevel::Error, "Fail Read Json: JGObject");
	}

	ReadJson(json);
}
