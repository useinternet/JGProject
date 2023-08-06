#pragma once
#include "Object/JGObject.h"
#include "String/Name.h"
#include "Object/ObjectGlobals.h"
#include "FileIO/Json.h"
#include "Runtime/CodeGen/Module.Core.DevelopUnit.generation.h"

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

class WWidgetComponent;

JGCLASS()
class JGDevelopUnit : public JGObject
{
	JG_GENERATED_CLASS_BODY
public:
	virtual ~JGDevelopUnit() = default;

public:
	virtual void Startup() {};
	virtual void Update() {};
	virtual void Shutdown() {};

	// À§Á¬
	virtual PSharedPtr<WWidgetComponent> CreateContentWidgetComponent() { return nullptr; }

	static JGDevelopUnit* LoadDevelopUnit(PSharedPtr<JGClass> inClass, const HGuid& inGuid = HGuid());
	static bool ReloadDevelopUnit(JGDevelopUnit*& inDevelopUnit);
	static void UnloadDevelopUnit(JGDevelopUnit*& outDevelopUnit, bool bRevemoDll = false);
private:
	PName _dllName;
	HGuid _guid;
	HJInstance _dllInstance;
};

