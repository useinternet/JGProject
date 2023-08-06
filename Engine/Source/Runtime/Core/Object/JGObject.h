#pragma once
#include "CoreDefines.h"
#include "ObjectDefines.h"
#include "FileIO/Jsonable.h"
#include "String/Name.h"
#include "Memory/Memory.h"
#include "Misc/Delegate.h"

class JGType;
class JGClass;
class ITypeable
{
public:
	virtual JGType GetType() const = 0;
};

JGCLASS()
class JGObject : public IMemoryObject, public IJsonable, public ITypeable
{
	JG_GENERATED_CLASS_BODY
private:
	mutable PName _name;

public:
	virtual ~JGObject() = default;

public:
	void SetName(const PName& name);
	const PName& GetName() const;

	PSharedPtr<JGClass> GetClass() const;
protected:
	virtual void OnWriteJson(PJsonData& json) const {}
	virtual void OnReadJson(const PJsonData& json) {}

	virtual void WriteJson(PJsonData& json) const override;
	virtual void ReadJson(const PJsonData& json) override;
};

