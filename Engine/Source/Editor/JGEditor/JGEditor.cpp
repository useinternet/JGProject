#include "PCH/PCH.h"
#include "JGEditor.h"
#include "Platform/Platform.h"
#include "Platform/JWindow.h"

#ifdef _PLATFORM_WINDOWS
#include "Platform/Windows/WindowsJWindow.h"
#endif

JG_MODULE_IMPL(HJGEditorModule, JGEDITOR_C_API)

HJGEditorModule::~HJGEditorModule()
{
	
}

JGType HJGEditorModule::GetModuleType() const
{
	return JGTYPE(HJGEditorModule);
}

void HJGEditorModule::StartupModule()
{
	// Window 생성
	HJWindowArguments winArgs;
	winArgs.Title = "JGEditor";
	winArgs.Size  = HVector2Int(800, 600);
	_window = HPlatform::CreateJWindow(winArgs);

	// Scheuler에 업데이트


	JG_LOG(JGEditor, ELogLevel::Trace, "Startup JGEditor Module...");
}

void HJGEditorModule::ShutdownModule()
{
	_window = nullptr;

	JG_LOG(JGEditor, ELogLevel::Trace, "Shutdown JGEditor Module...");
}


