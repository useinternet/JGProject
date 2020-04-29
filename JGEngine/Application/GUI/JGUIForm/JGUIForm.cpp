#include "pch.h"
#include "JGUIForm.h"

#include <filesystem>
#include "GUI/JGUIComponent/Element/JGUIRenderer.h"
#include "GUI/JGUIComponent/Script/Button.h"
#include "GUI/JGUIComponent/Script/Text.h"

#include "GUI/JGUIComponent/Script/MenuItem.h"
#include "GUI/JGUIComponent/Script/MainMenu.h"
using namespace std;

static int cnt = 0;




void JGUIForm::Awake()
{
	//JGUIWindow::Awake();
	//if (JGUI::GetMainWindow() == nullptr)
	//{
	//	//auto flag = GetWindowFlags();

	//	//SetWindowFlags(flag |
	//	//	JGUI_WindowFlag_MainWindow | JGUI_WindowFlag_EnableClose |
	//	//	JGUI_WindowFlag_EnableMaximize | JGUI_WindowFlag_EnableMinimize |
	//	//	JGUI_WindowFlag_MenuBar);
	//}
	//else
	//{
	//	SetWindowFlags(GetWindowFlags());
	//}
}

void JGUIForm::Start()
{
	//auto menu = GetMenu();
	//if (menu)
	//{
	//	// file
	//	{
	//		MainMenuItemDesc mainDesc;
	//		mainDesc.auxiliary_key = KeyCode::F;
	//		mainDesc.contents = "File";

	//		auto file = menu->AddItem(mainDesc);


	//		MenuItemDesc desc;

	//		desc.contents = "Open";
	//		desc.auxiliary_key = KeyCode::A;
	//		file->AddItem(desc);


	//		desc.contents = "Copy";
	//		desc.shortcut_key.push_back(KeyCode::Ctrl);
	//		desc.shortcut_key.push_back(KeyCode::A);
	//		file->AddItem(desc);
	//	}
	//	// tool

	//	{
	//		MainMenuItemDesc mainDesc;
	//		mainDesc.contents = "Tool";

	//		auto tool = menu->AddItem(mainDesc);


	//		MenuItemDesc desc;
	//		desc.contents = "Log";
	//		tool->AddItem(desc);

	//		desc.contents = "TestTest";
	//		desc.shortcut_key.push_back(KeyCode::A);
	//		desc.shortcut_key.push_back(KeyCode::C);
	//		auto test = tool->AddItem(desc);


	//		desc.shortcut_key.clear();
	//		desc.contents = "Test1";
	//		test->AddItem(desc);
	//		desc.contents = "Test2";
	//		test->AddItem(desc);
	//		desc.contents = "Test3";
	//		test->AddItem(desc);
	//	}
	//	//Help
	//	{
	//		MainMenuItemDesc mainDesc;
	//		mainDesc.contents = "Help";

	//		auto help = menu->AddItem(mainDesc);

	//		MenuItemDesc desc;
	//		desc.contents = "CreateWindow";
	//		desc.shortcut_key.push_back(KeyCode::Ctrl);
	//		desc.shortcut_key.push_back(KeyCode::A);
	//		desc.func = [&](void* data)
	//		{
	//			auto testwindow = CreateJGUIForm<JGUIForm>("TestForm" + to_string(cnt++), JGUI_WindowFlag_NewLoad |
	//				JGUI_WindowFlag_ChildWindow | JGUI_WindowFlag_EnableClose);
	//			testwindow->GetTransform()->SetSize(300, 300);
	//			testwindow->GetTransform()->SetLocalPosition(100, 100);
	//		};
	//		help->AddItem(desc);
	//	}



	//}


}

void JGUIForm::Resize(const JGUIResizeEvent& e)
{


}
void JGUIForm::Tick(const JGUITickEvent& e)
{


}
void JGUIForm::KeyDown(const JGUIKeyDownEvent& e)
{

}
void JGUIForm::KeyUp(const JGUIKeyUpEvent& e)
{

	//// TestCode
	//if (e.Code == KeyCode::A)
	//{
	//	auto testwindow = CreateJGUIForm<JGUIForm>("TestForm" + to_string(cnt++), JGUI_WindowFlag_NewLoad |
	//		JGUI_WindowFlag_MainWindow | JGUI_WindowFlag_EnableClose);
	//	testwindow->GetTransform()->SetSize(300, 300);
	//	testwindow->GetTransform()->SetLocalPosition(100, 100);
	//}
	//if (e.Code == KeyCode::B)
	//{
	//	auto testwindow = CreateJGUIForm<JGUIForm>("TestForm" + to_string(cnt++), JGUI_WindowFlag_NewLoad |
	//		JGUI_WindowFlag_ChildWindow | JGUI_WindowFlag_EnableClose);
	//	testwindow->GetTransform()->SetSize(300, 300);
	//	testwindow->GetTransform()->SetLocalPosition(100, 100);
	//}
	//if (e.Code == KeyCode::End)
	//{
	//	SetParent(nullptr);
	//}
}
void JGUIForm::MouseWheel(const JGUIMouseWheelEvent& e)
{
}





