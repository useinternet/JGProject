#include "PCH/PCH.h"
#include "JGEditor.h"
#include "Platform/Platform.h"
#include "Platform/JWindow.h"
#include "GUI.h"

#ifdef _PLATFORM_WINDOWS
#include "Platform/Windows/WindowsJWindow.h"
#endif


#include "Widgets/Develop/WDevelopWidget.h"

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
	// Window »ý¼º
	HJWindowArguments winArgs;
	winArgs.Title = "JGEditor";
	winArgs.Size  = HVector2Int(800, 600);
	_window = HPlatform::CreateJWindow(winArgs);

	GCoreSystem::GetGlobalValues().MainWindow = _window.GetRawPointer();

	GCoreSystem::RegisterSystemInstance<GGUIGlobalSystem>();

	if (GModuleGlobalSystem::GetInstance().ConnectModule("Graphics") == false)
	{
		JG_LOG(JGEditor, ELogLevel::Critical, "Fail Connect Graphics Module...");
	}

	JG_LOG(JGEditor, ELogLevel::Trace, "Startup JGEditor Module...");

	GGUIGlobalSystem::GetInstance().GUIEvents.OnMenuBuild.AddRaw(this, &HJGEditorModule::BuildMainMenu);
}

void HJGEditorModule::ShutdownModule()
{
	_window = nullptr;

	GModuleGlobalSystem::GetInstance().DisconnectModule("Graphics");

	GCoreSystem::UnRegisterSystemInstance<GGUIGlobalSystem>();

	JG_LOG(JGEditor, ELogLevel::Trace, "Shutdown JGEditor Module...");
}

void HJGEditorModule::BuildMainMenu(HMenuBuilder& inMenuBuilder)
{
	PName SectionName = PName("Test");
	inMenuBuilder.AddSection(SectionName, 0);
	inMenuBuilder.AddMenuPath(SectionName, PName("Files"));
	inMenuBuilder.AddMenuPath(SectionName, PName("Windows"));
	

	//inMenuBuilder.AddSection(SectionName, 0);
	//inMenuBuilder.AddMainMenu(w)

}

