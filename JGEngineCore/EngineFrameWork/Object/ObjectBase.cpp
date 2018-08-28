#include "ObjectBase.h"
const char* ObjectBase::ObjectID;

ObjectBase::ObjectBase()
{

}
ObjectBase::~ObjectBase()
{
}


void ObjectBase::RegisterObjectID(const type_info& ID)
{
	static bool bInit = false;
	if (!bInit)
	{
		bInit = true;
		ObjectID = ID.name();
	}

}

const char* ObjectBase::GetID()
{
	return ObjectID;
}
