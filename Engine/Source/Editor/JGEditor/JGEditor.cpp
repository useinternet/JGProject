#include "PCH/PCH.h"
#include "JGEditor.h"

using namespace std;

JG_MODULE_IMPL(HJGEditorModule, JGEDITOR_C_API)

void HJGEditorModule::StartupModule()
{
	// Window »ý¼º
	JG_LOG(JGEditor, ELogLevel::Trace, "Startup JGEditor Module...");
}

void HJGEditorModule::ShutdownModule()
{
	JG_LOG(JGEditor, ELogLevel::Trace, "Shutdown JGEditor Module...");
}


