#include "PCH/PCH.h"
#include "Core.h"
#include "Runtime/GUI/Widget.h"


PSharedPtr<JGClass> Module_GUI_Code_Generation_Static_Create_Class_WWidget()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGType::GenerateType<HGuid>(),"_widgetGuid",nullptr));
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGType::GenerateType<bool>(),"_bIsOpen",nullptr));
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGType::GenerateType<WWidget>(),{
			JGType::GenerateType<JGObject>(), 
		},
		PropertyMap, FunctionMap,nullptr);
}

PSharedPtr<JGClass> Module_GUI_Code_Generation_Create_Class_WWidget(const JGObject* fromThis)
{
	WWidget* noneConstThisPtr = const_cast<WWidget*>(static_cast<const WWidget*>(fromThis));
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(noneConstThisPtr, GObjectGlobalSystem::GetInstance().GetStaticClass(JGType::GenerateType<WWidget>()));

	if (Class->HasProperty(PName("_widgetGuid")) == true)
	{
		if (PObjectGlobalsPrivateUtils::BindProperty(noneConstThisPtr, Class->FindProperty(PName("_widgetGuid")), &(noneConstThisPtr->_widgetGuid)) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "WWidget: Fail Bind Property : _widgetGuid");
		}
	}



	if (Class->HasProperty(PName("_bIsOpen")) == true)
	{
		if (PObjectGlobalsPrivateUtils::BindProperty(noneConstThisPtr, Class->FindProperty(PName("_bIsOpen")), &(noneConstThisPtr->_bIsOpen)) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "WWidget: Fail Bind Property : _bIsOpen");
		}
	}


	return Class;

}

PSharedPtr<JGObject> Module_GUI_Code_Generation_Create_Object_WWidget()
{
	return Allocate<WWidget>();
}

