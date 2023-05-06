#include "PCH/PCH.h"
#include "Core.h"
#include "Runtime/GUI/Datas/GUIData.h"


PSharedPtr<JGClass> Module_GUI_Code_Generation_Static_Create_Class_JGGUIData()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGType::GenerateType<HList<PSharedPtr<WWidget>>>(),"Widgets",nullptr));
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGType::GenerateType<HHashMap<HGuid,PName>>(),"RedirectMap",nullptr));
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGType::GenerateType<JGGUIData>(),{
			JGType::GenerateType<JGObject>(), 
		},
		PropertyMap, FunctionMap,nullptr);
}

PSharedPtr<JGClass> Module_GUI_Code_Generation_Create_Class_JGGUIData(const JGObject* fromThis)
{
	JGGUIData* noneConstThisPtr = const_cast<JGGUIData*>(static_cast<const JGGUIData*>(fromThis));
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(noneConstThisPtr, GObjectGlobalSystem::GetInstance().GetStaticClass(JGType::GenerateType<JGGUIData>()));

	if (Class->HasProperty(PName("Widgets")) == true)
	{
		if (PObjectGlobalsPrivateUtils::BindProperty(noneConstThisPtr, Class->FindProperty(PName("Widgets")), &(noneConstThisPtr->Widgets)) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGGUIData: Fail Bind Property : Widgets");
		}
	}



	if (Class->HasProperty(PName("RedirectMap")) == true)
	{
		if (PObjectGlobalsPrivateUtils::BindProperty(noneConstThisPtr, Class->FindProperty(PName("RedirectMap")), &(noneConstThisPtr->RedirectMap)) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGGUIData: Fail Bind Property : RedirectMap");
		}
	}


	return Class;

}

PSharedPtr<JGObject> Module_GUI_Code_Generation_Create_Object_JGGUIData()
{
	return Allocate<JGGUIData>();
}

