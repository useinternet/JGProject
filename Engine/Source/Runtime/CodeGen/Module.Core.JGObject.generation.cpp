#include "PCH/PCH.h"
#include "Core.h"
#include "Runtime/Core/Object/JGObject.h"


PSharedPtr<JGClass> Module_Core_Code_Generation_Static_Create_Class_JGObject()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGType::GenerateType<JGObject>(),{
			JGType::GenerateType<IMemoryObject>(), 
			JGType::GenerateType<IJsonable>(), 
			JGType::GenerateType<ITypeable>(), 
		},
		PropertyMap, FunctionMap,nullptr);
}

PSharedPtr<JGClass> Module_Core_Code_Generation_Create_Class_JGObject(const JGObject* fromThis)
{
	JGObject* noneConstThisPtr = const_cast<JGObject*>(static_cast<const JGObject*>(fromThis));
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(noneConstThisPtr, GObjectGlobalSystem::GetInstance().GetStaticClass(JGType::GenerateType<JGObject>()));
	return Class;

}

PSharedPtr<JGObject> Module_Core_Code_Generation_Create_Object_JGObject()
{
	return Allocate<JGObject>();
}

