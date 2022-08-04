#include "JGObject.h"

void JGObject::SetName(const PString& name)
{
	_name = name;
}

const PString& JGObject::GetName() const
{
	return _name;
}
