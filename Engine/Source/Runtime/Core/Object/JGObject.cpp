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
