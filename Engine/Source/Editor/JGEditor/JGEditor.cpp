#include "PCH/PCH.h"
#include "JGEditor.h"
#include "Platform/Platform.h"
#include "Platform/JWindow.h"

#ifdef _PLATFORM_WINDOWS
#include "Platform/Windows/WindowsJWindow.h"
#endif

JG_MODULE_IMPL(HJGEditorModule, JGEDITOR_C_API)

JGType HJGEditorModule::GetModuleType() const
{
	return JGTYPE(HJGEditorModule);
}

void HJGEditorModule::StartupModule()
{
	// Window »ý¼º
	HJWindowArguments winArgs;
	winArgs.Title = "JGEditor";
	winArgs.Size  = HVector2Int(800, 600);
	HPlatform::CreateJWindow(winArgs);

	JG_LOG(JGEditor, ELogLevel::Trace, "Startup JGEditor Module...");
}

void HJGEditorModule::ShutdownModule()
{
	JG_LOG(JGEditor, ELogLevel::Trace, "Shutdown JGEditor Module...");
}


