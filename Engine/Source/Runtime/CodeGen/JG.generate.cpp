#include "PCH/PCH.h"
#include "CoreSystem.h"
#include "Object/ObjectGlobalSystem.h"
#include "Object/ObjectGlobals.h"
#include "Misc/Log.h"


#ifdef _CODEGEN
#define CODEGEN_API extern "C" __declspec(dllexport)
#else
#define CODEGEN_API extern "C" __declspec(dllimport)
#endif


CODEGEN_API void Link_Module(GCoreSystem* ins)
{
	HCoreSystemPrivate::SetInstance(ins);
}



extern PSharedPtr<JGClass> Module_DevelopUnitTest_Code_Generation_Static_Create_Class_JGDUT_Graphics();
extern PSharedPtr<JGClass> Module_DevelopUnitTest_Code_Generation_Create_Class_JGDUT_Graphics(const JGObject* fromThis);
extern PSharedPtr<JGObject> Module_DevelopUnitTest_Code_Generation_Create_Object_JGDUT_Graphics();
extern PSharedPtr<JGClass> Module_JGEditor_Code_Generation_Static_Create_Class_WDevelopWidget();
extern PSharedPtr<JGClass> Module_JGEditor_Code_Generation_Create_Class_WDevelopWidget(const JGObject* fromThis);
extern PSharedPtr<JGObject> Module_JGEditor_Code_Generation_Create_Object_WDevelopWidget();
extern PSharedPtr<JGClass> Module_JGConsole_Code_Generation_Static_Create_Class_JGTestClassA();
extern PSharedPtr<JGClass> Module_JGConsole_Code_Generation_Create_Class_JGTestClassA(const JGObject* fromThis);
extern PSharedPtr<JGObject> Module_JGConsole_Code_Generation_Create_Object_JGTestClassA();
extern PSharedPtr<JGClass> Module_Core_Code_Generation_Static_Create_Class_JGDevelopUnit();
extern PSharedPtr<JGClass> Module_Core_Code_Generation_Create_Class_JGDevelopUnit(const JGObject* fromThis);
extern PSharedPtr<JGObject> Module_Core_Code_Generation_Create_Object_JGDevelopUnit();
extern PSharedPtr<JGClass> Module_Core_Code_Generation_Static_Create_Class_JGObject();
extern PSharedPtr<JGClass> Module_Core_Code_Generation_Create_Class_JGObject(const JGObject* fromThis);
extern PSharedPtr<JGObject> Module_Core_Code_Generation_Create_Object_JGObject();
extern PSharedPtr<JGClass> Module_GUI_Code_Generation_Static_Create_Class_JGGUIData();
extern PSharedPtr<JGClass> Module_GUI_Code_Generation_Create_Class_JGGUIData(const JGObject* fromThis);
extern PSharedPtr<JGObject> Module_GUI_Code_Generation_Create_Object_JGGUIData();
extern PSharedPtr<JGClass> Module_GUI_Code_Generation_Static_Create_Class_WWidget();
extern PSharedPtr<JGClass> Module_GUI_Code_Generation_Create_Class_WWidget(const JGObject* fromThis);
extern PSharedPtr<JGObject> Module_GUI_Code_Generation_Create_Object_WWidget();
extern PSharedPtr<JGEnum> Module_JGConsole_Code_Generation_Static_Create_Enum_ETestEnumA();
extern PSharedPtr<JGEnum> Module_Core_Code_Generation_Static_Create_Enum_EKeyCode();
extern PSharedPtr<JGEnum> Module_Graphics_Code_Generation_Static_Create_Enum_ETextureFormat();
extern PSharedPtr<JGEnum> Module_Graphics_Code_Generation_Static_Create_Enum_EResourceState();
extern PSharedPtr<JGEnum> Module_Graphics_Code_Generation_Static_Create_Enum_ECommandQueueType();


CODEGEN_API bool Engine_CodeGenerate(GObjectGlobalSystem* objectGlobalSystem)
{

    objectGlobalSystem->RegisterJGClass(Module_DevelopUnitTest_Code_Generation_Static_Create_Class_JGDUT_Graphics(), Module_DevelopUnitTest_Code_Generation_Create_Class_JGDUT_Graphics, Module_DevelopUnitTest_Code_Generation_Create_Object_JGDUT_Graphics);
    objectGlobalSystem->RegisterJGClass(Module_JGEditor_Code_Generation_Static_Create_Class_WDevelopWidget(), Module_JGEditor_Code_Generation_Create_Class_WDevelopWidget, Module_JGEditor_Code_Generation_Create_Object_WDevelopWidget);
    objectGlobalSystem->RegisterJGClass(Module_JGConsole_Code_Generation_Static_Create_Class_JGTestClassA(), Module_JGConsole_Code_Generation_Create_Class_JGTestClassA, Module_JGConsole_Code_Generation_Create_Object_JGTestClassA);
    objectGlobalSystem->RegisterJGClass(Module_Core_Code_Generation_Static_Create_Class_JGDevelopUnit(), Module_Core_Code_Generation_Create_Class_JGDevelopUnit, Module_Core_Code_Generation_Create_Object_JGDevelopUnit);
    objectGlobalSystem->RegisterJGClass(Module_Core_Code_Generation_Static_Create_Class_JGObject(), Module_Core_Code_Generation_Create_Class_JGObject, Module_Core_Code_Generation_Create_Object_JGObject);
    objectGlobalSystem->RegisterJGClass(Module_GUI_Code_Generation_Static_Create_Class_JGGUIData(), Module_GUI_Code_Generation_Create_Class_JGGUIData, Module_GUI_Code_Generation_Create_Object_JGGUIData);
    objectGlobalSystem->RegisterJGClass(Module_GUI_Code_Generation_Static_Create_Class_WWidget(), Module_GUI_Code_Generation_Create_Class_WWidget, Module_GUI_Code_Generation_Create_Object_WWidget);
    objectGlobalSystem->RegisterJGEnum(Module_JGConsole_Code_Generation_Static_Create_Enum_ETestEnumA());
    objectGlobalSystem->RegisterJGEnum(Module_Core_Code_Generation_Static_Create_Enum_EKeyCode());
    objectGlobalSystem->RegisterJGEnum(Module_Graphics_Code_Generation_Static_Create_Enum_ETextureFormat());
    objectGlobalSystem->RegisterJGEnum(Module_Graphics_Code_Generation_Static_Create_Enum_EResourceState());
    objectGlobalSystem->RegisterJGEnum(Module_Graphics_Code_Generation_Static_Create_Enum_ECommandQueueType());


    return true;
}
