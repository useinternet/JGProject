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



extern PSharedPtr<JGClass> Module_Core_Code_Generation_Static_Create_Class_PTestObject();
extern PSharedPtr<JGClass> Module_Core_Code_Generation_Create_Class_PTestObject(const JGObject* fromThis);


CODEGEN_API bool Engine_CodeGenerate(GObjectGlobalSystem* objectGlobalSystem)
{

    objectGlobalSystem->RegisterJGClass(Module_Core_Code_Generation_Static_Create_Class_PTestObject(), Module_Core_Code_Generation_Create_Class_PTestObject);


    return true;
}
