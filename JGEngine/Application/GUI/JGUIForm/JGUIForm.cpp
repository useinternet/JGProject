#include "pch.h"
#include "JGUIForm.h"

#include <filesystem>
#include "GUI/JGUIComponent/Element/JGUIRenderer.h"
#include "GUI/JGUIComponent/Script/Button.h"
#include "GUI/JGUIComponent/Script/Text.h"

#include "GUI/JGUIComponent/Script/MenuItem.h"
using namespace std;

static int cnt = 0;




void JGUIForm::Awake()
{
	JGUIWindow::Awake();
	if (JGUI::GetMainWindow() == nullptr)
	{
		auto flag = GetWindowFlags();

		SetWindowFlags(flag |
			JGUI_WindowFlag_MainWindow | JGUI_WindowFlag_EnableClose |
			JGUI_WindowFlag_EnableMaximize | JGUI_WindowFlag_EnableMinimize |
			JGUI_WindowFlag_MenuBar | JGUI_WindowFlag_VerticalScrollbar |
		JGUI_WindowFlag_HorizontalScrollbar);
	}
	else
	{
		SetWindowFlags(GetWindowFlags());
	}
}


void JGUIForm::Start()
{
	auto menuitem = CreateJGUIElement("Item")->CreateJGUIComponent<MenuItem>();
	MenuItemDesc desc;
	desc.name     = "TestItem1";
	desc.contents = "Copy";
	desc.auxiliary_key = KeyCode::C;
	desc.shortcut_key.push_back(KeyCode::Ctrl);
	desc.shortcut_key.push_back(KeyCode::A);
	desc.shortcut_key.push_back(KeyCode::B);
	desc.icon_source_path = "Close.png";
	menuitem->m_Desc = desc;
	menuitem->Setting();
	menuitem->GetTransform()->SetLocalPosition(300, 200);



	auto submenu1 = menuitem->AddItem(desc);
	desc.shortcut_key.clear();
	desc.contents = "contents";
	menuitem->AddSeparater();
	auto submenu2 = menuitem->AddItem(desc);


	submenu1->AddItem(desc);
	submenu1->AddItem(desc);
	submenu1->AddItem(desc);

	submenu2->AddItem(desc);
	submenu2->AddItem(desc);
	//menuitem->OpenItems();

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

	// TestCode
	if (e.Code == KeyCode::A)
	{
		auto testwindow = CreateJGUIForm<JGUIForm>("TestForm" + to_string(cnt++), JGUI_WindowFlag_NewLoad |
			JGUI_WindowFlag_MainWindow | JGUI_WindowFlag_EnableClose);
		testwindow->GetTransform()->SetSize(300, 300);
		testwindow->GetTransform()->SetLocalPosition(100, 100);
	}
	if (e.Code == KeyCode::B)
	{
		auto testwindow = CreateJGUIForm<JGUIForm>("TestForm" + to_string(cnt++), JGUI_WindowFlag_NewLoad |
			JGUI_WindowFlag_ChildWindow | JGUI_WindowFlag_EnableClose);
		testwindow->GetTransform()->SetSize(300, 300);
		testwindow->GetTransform()->SetLocalPosition(100, 100);
	}
	if (e.Code == KeyCode::End)
	{
		SetParent(nullptr);
	}
}
void JGUIForm::MouseWheel(const JGUIMouseWheelEvent& e)
{
}





