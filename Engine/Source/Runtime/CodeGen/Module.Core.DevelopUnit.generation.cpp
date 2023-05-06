#include "PCH/PCH.h"
#include "Core.h"
#include "Runtime/Core/Misc/DevelopUnit.h"


PSharedPtr<JGClass> Module_Core_Code_Generation_Static_Create_Class_JGDevelopUnit()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGType::GenerateType<JGDevelopUnit>(),{
			JGType::GenerateType<JGObject>(), 
		},
		PropertyMap, FunctionMap,nullptr);
}

PSharedPtr<JGClass> Module_Core_Code_Generation_Create_Class_JGDevelopUnit(const JGObject* fromThis)
{
	JGDevelopUnit* noneConstThisPtr = const_cast<JGDevelopUnit*>(static_cast<const JGDevelopUnit*>(fromThis));
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(noneConstThisPtr, GObjectGlobalSystem::GetInstance().GetStaticClass(JGType::GenerateType<JGDevelopUnit>()));
	return Class;

}

PSharedPtr<JGObject> Module_Core_Code_Generation_Create_Object_JGDevelopUnit()
{
	return Allocate<JGDevelopUnit>();
}

