#include "PCH/PCH.h"
#include "JGEditor.h"
#include "Platform/Platform.h"
#include "Platform/JWindow.h"
#include "GUI.h"

#ifdef _PLATFORM_WINDOWS
#include "Platform/Windows/WindowsJWindow.h"
#endif


#include "Widgets/Develop/DevelopWidget.h"

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

	GCoreSystem::GetGlobalValues().MainWindow = _window.GetRawPointer();

	if (GModuleGlobalSystem::GetInstance().ConnectModule("Graphics") == false)
	{
		JG_LOG(JGEditor, ELogLevel::Critical, "Fail Connect Graphics Module...");
	}
	// Scheuler에 업데이트
	// Graphics 연결
	GCoreSystem::RegisterSystemInstance<GGUIGlobalSystem>();
	JG_LOG(JGEditor, ELogLevel::Trace, "Startup JGEditor Module...");

	GGUIGlobalSystem::GetInstance().OpenWidget<WDevelopWidget>();

}

void HJGEditorModule::ShutdownModule()
{
	_window = nullptr;
	GCoreSystem::UnRegisterSystemInstance<GGUIGlobalSystem>();
	JG_LOG(JGEditor, ELogLevel::Trace, "Shutdown JGEditor Module...");
}


