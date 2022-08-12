#include "JGObject.h"
#include "ObjectGlobals.h"

JGType JGObject::GetType() const
{
	return JGTYPE(JGObject);
}

void JGObject::SetName(const PString& name)
{
	_name = name;
}

const PString& JGObject::GetName() const
{
	return _name;
}
