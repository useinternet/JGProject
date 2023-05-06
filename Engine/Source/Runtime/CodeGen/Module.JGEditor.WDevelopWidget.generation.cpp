#include "PCH/PCH.h"
#include "Core.h"
#include "Editor/JGEditor/Widgets/Develop/WDevelopWidget.h"


PSharedPtr<JGClass> Module_JGEditor_Code_Generation_Static_Create_Class_WDevelopWidget()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGType::GenerateType<WDevelopWidget>(),{
			JGType::GenerateType<WWidget>(), 
		},
		PropertyMap, FunctionMap,nullptr);
}

PSharedPtr<JGClass> Module_JGEditor_Code_Generation_Create_Class_WDevelopWidget(const JGObject* fromThis)
{
	WDevelopWidget* noneConstThisPtr = const_cast<WDevelopWidget*>(static_cast<const WDevelopWidget*>(fromThis));
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(noneConstThisPtr, GObjectGlobalSystem::GetInstance().GetStaticClass(JGType::GenerateType<WDevelopWidget>()));
	return Class;

}

PSharedPtr<JGObject> Module_JGEditor_Code_Generation_Create_Object_WDevelopWidget()
{
	return Allocate<WDevelopWidget>();
}

