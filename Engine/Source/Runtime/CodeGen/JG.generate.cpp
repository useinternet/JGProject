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



extern PSharedPtr<JGClass> Module_JGConsole_Code_Generation_Static_Create_Class_JGTestClassA();
extern PSharedPtr<JGClass> Module_JGConsole_Code_Generation_Create_Class_JGTestClassA(const JGObject* fromThis);
extern PSharedPtr<JGEnum> Module_JGConsole_Code_Generation_Static_Create_Enum_ETestEnumA();


CODEGEN_API bool Engine_CodeGenerate(GObjectGlobalSystem* objectGlobalSystem)
{

    objectGlobalSystem->RegisterJGClass(Module_JGConsole_Code_Generation_Static_Create_Class_JGTestClassA(), Module_JGConsole_Code_Generation_Create_Class_JGTestClassA);
    objectGlobalSystem->RegisterJGEnum(Module_JGConsole_Code_Generation_Static_Create_Enum_ETestEnumA());


    return true;
}
