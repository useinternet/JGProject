#pragma once
#include "Object/JGObject.h"
#pragma warning(disable : 4275)

#define JG_REGISTER_DEVELOP_UNIT(UnitName, APIDefine) \
APIDefine JGDevelopUnit* _Create_DevelopUnit_() \
{ \
	return HPlatform::Allocate<##UnitName>(); \
} \
APIDefine void _Link_DevelopUnit_(GCoreSystem* ins) \
{ \
	HCoreSystemPrivate::SetInstance(ins); \
} \

#define JG_CREATE_DEVELOPUNIT_FUNCTION_NAME "_Create_DevelopUnit_"
#define JG_LINK_DEVELOPUNIT_FUNCTION_NAME   "_Link_DevelopUnit_"

JGCLASS()
class JGDevelopUnit : public JGObject
{
public:
	

public:
	virtual ~JGDevelopUnit() = default;

public:
	virtual void Startup()  = 0;
	virtual void Update()   = 0;
	virtual void Shutdown() = 0;
};

