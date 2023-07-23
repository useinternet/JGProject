#include "PCH/PCH.h"
#include "Core.h"
#include "Editor/JGEditor/Datas/DevelopUnitListData.h"


PSharedPtr<JGClass> Module_JGEditor_Code_Generation_Static_Create_Class_JGDevelopUnitListData()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGType::GenerateType<JGDevelopUnitListData>(),{
			JGType::GenerateType<JGObject>(), 
		},
		PropertyMap, FunctionMap,nullptr);
}

PSharedPtr<JGClass> Module_JGEditor_Code_Generation_Create_Class_JGDevelopUnitListData(const JGObject* fromThis)
{
	JGDevelopUnitListData* noneConstThisPtr = const_cast<JGDevelopUnitListData*>(static_cast<const JGDevelopUnitListData*>(fromThis));
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(noneConstThisPtr, GObjectGlobalSystem::GetInstance().GetStaticClass(JGType::GenerateType<JGDevelopUnitListData>()));
	return Class;

}

PSharedPtr<JGObject> Module_JGEditor_Code_Generation_Create_Object_JGDevelopUnitListData()
{
	return Allocate<JGDevelopUnitListData>();
}

