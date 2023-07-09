#include "PCH/PCH.h"
#include "Core.h"
#include "Runtime/GUI/GUIStyles/GenericGUIStyle.h"


PSharedPtr<JGClass> Module_GUI_Code_Generation_Static_Create_Class_JGGenericGUIStyle()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGType::GenerateType<JGGenericGUIStyle>(),{
			JGType::GenerateType<JGGUIStyle>(), 
		},
		PropertyMap, FunctionMap,nullptr);
}

PSharedPtr<JGClass> Module_GUI_Code_Generation_Create_Class_JGGenericGUIStyle(const JGObject* fromThis)
{
	JGGenericGUIStyle* noneConstThisPtr = const_cast<JGGenericGUIStyle*>(static_cast<const JGGenericGUIStyle*>(fromThis));
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(noneConstThisPtr, GObjectGlobalSystem::GetInstance().GetStaticClass(JGType::GenerateType<JGGenericGUIStyle>()));
	return Class;

}

PSharedPtr<JGObject> Module_GUI_Code_Generation_Create_Object_JGGenericGUIStyle()
{
	return Allocate<JGGenericGUIStyle>();
}

