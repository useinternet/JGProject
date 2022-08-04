#include "Object/ObjectGlobalSystem.h"
#include "Object/ObjectGlobals.h"

#include "Misc/Log.h"

CODE_GENERATION_INCLUDE_BEGIN()
extern PSharedPtr<JGStruct> Module_Core_Code_Generation_Create_Static_JGStruct_TestObject();

CODE_GENERATION_INCLUDE_END()

bool GObjectGlobalSystem::codeGen()
{
	JG_LOG(Core, ELogLevel::Info, "Begin Object Code Generation");
	CODE_GENERATION_BEGIN()

		registerStruct(Module_Core_Code_Generation_Create_Static_JGStruct_TestObject());


	CODE_GENERATION_END()
	JG_LOG(Core, ELogLevel::Info, "End Object Code Generation");

	return true;
}