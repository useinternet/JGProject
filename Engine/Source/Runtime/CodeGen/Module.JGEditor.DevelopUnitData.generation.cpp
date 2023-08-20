#include "PCH/PCH.h"
#include "Core.h"
#include "Editor/JGEditor/Datas/DevelopUnitData.h"


PSharedPtr<JGClass> Module_JGEditor_Code_Generation_Static_Create_Class_JGDevelopUnitData()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGType::GenerateType<HGuid>(),"UnitGuid",nullptr));
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGType::GenerateType<JGType>(),"UnitType",nullptr));
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGType::GenerateType<JGDevelopUnitData>(),{
			JGType::GenerateType<JGObject>(), 
		},
		PropertyMap, FunctionMap,nullptr);
}

PSharedPtr<JGClass> Module_JGEditor_Code_Generation_Create_Class_JGDevelopUnitData(const JGObject* fromThis)
{
	JGDevelopUnitData* noneConstThisPtr = const_cast<JGDevelopUnitData*>(static_cast<const JGDevelopUnitData*>(fromThis));
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(noneConstThisPtr, GObjectGlobalSystem::GetInstance().GetStaticClass(JGType::GenerateType<JGDevelopUnitData>()));

	if (Class->HasProperty(PName("UnitGuid")) == true)
	{
		if (PObjectGlobalsPrivateUtils::BindProperty(noneConstThisPtr, Class->FindProperty(PName("UnitGuid")), &(noneConstThisPtr->UnitGuid)) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGDevelopUnitData: Fail Bind Property : UnitGuid");
		}
	}



	if (Class->HasProperty(PName("UnitType")) == true)
	{
		if (PObjectGlobalsPrivateUtils::BindProperty(noneConstThisPtr, Class->FindProperty(PName("UnitType")), &(noneConstThisPtr->UnitType)) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGDevelopUnitData: Fail Bind Property : UnitType");
		}
	}


	return Class;

}

PSharedPtr<JGObject> Module_JGEditor_Code_Generation_Create_Object_JGDevelopUnitData()
{
	return Allocate<JGDevelopUnitData>();
}

