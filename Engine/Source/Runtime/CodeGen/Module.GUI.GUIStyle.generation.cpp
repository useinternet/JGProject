#include "PCH/PCH.h"
#include "Core.h"
#include "Runtime/GUI/GUIStyles/GUIStyle.h"


PSharedPtr<JGClass> Module_GUI_Code_Generation_Static_Create_Class_JGGUIStyle()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGType::GenerateType<JGGUIStyle>(),{
			JGType::GenerateType<JGObject>(), 
		},
		PropertyMap, FunctionMap,nullptr);
}

PSharedPtr<JGClass> Module_GUI_Code_Generation_Create_Class_JGGUIStyle(const JGObject* fromThis)
{
	JGGUIStyle* noneConstThisPtr = const_cast<JGGUIStyle*>(static_cast<const JGGUIStyle*>(fromThis));
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(noneConstThisPtr, GObjectGlobalSystem::GetInstance().GetStaticClass(JGType::GenerateType<JGGUIStyle>()));
	return Class;

}

PSharedPtr<JGObject> Module_GUI_Code_Generation_Create_Object_JGGUIStyle()
{
	return Allocate<JGGUIStyle>();
}

