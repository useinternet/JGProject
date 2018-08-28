#include"ComponentBase.h"

const char* ComponentBase::ComponentID;
ComponentBase::ComponentBase()
{
}

ComponentBase::~ComponentBase()
{
}

void ComponentBase::RegisterComponentID(const type_info & ID)
{
	static bool bInit = false;
	if (!bInit)
	{
		bInit = true;
		ComponentID = ID.name();
	}

}

const char * ComponentBase::GetID()
{
	return ComponentID;
}
