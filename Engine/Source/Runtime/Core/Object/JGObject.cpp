#include "PCH/PCH.h"
#include "JGObject.h"
#include "ObjectGlobals.h"
#include "ObjectGlobalSystem.h"
#include "FileIO/Json.h"

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

PSharedPtr<JGClass> JGObject::GetClass() const
{
	return GObjectGlobalSystem::GetInstance().GetClass(GetType(), this);
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

	OnReadJson(json);
}
