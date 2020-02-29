#include "pch.h"
#include "JGUIForm.h"
#include "GUI/JGUIComponent/JGUIShape.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUICollider.h"
#include "GUI/JGUIComponent/JGUITitleBar.h"
#include "GUI/JGUIComponent/JGUIResizeBox.h"
#include "GUI/JGUIComponent/JGUIInputText.h"
#include "GUI/JGUIComponent/JGUIMenu.h"
#include "GUI/JGUIComponent/JGUIMenuItem.h"
#include "GUI/JGUIComponent/JGUIText.h"
#include "GUI/JGUIComponent/JGUITab.h"
#include <filesystem>
using namespace std;

static int cnt = 0;
void JGUIForm::Awake()
{
	m_Border = CreateJGUIComponent<JGUIEmptyRectangle>("BorderRectangle");

	if (m_Border)
	{
		m_Border->GetTransform()->SetSize(GetTransform()->GetSize());
		m_Border->SetPriority(1);
		m_Border->SetColor(JColor(0, 0, 0, 1));
		m_Border->SetThickness(1.0);

		//offset_x = 1.5f;
		//offset_y = 1.5f;
	}

	auto flag = GetWindowFlags();
	SetWindowFlags(flag | JGUI_WindowFlag_EnableResize |
		JGUI_WindowFlag_TabBar | JGUI_WindowFlag_TitileBar);



	//


	
	m_Txt = CreateJGUIComponent<JGUIText>("testTxt");
	m_Txt->SetText("TestText\ntestText1\ntestTEXT2");
	m_Txt->GetTransform()->SetLocalPosition(300, 200);
	m_Txt->GetTransform()->SetSize(200, 200);
	m_Txt->SetTextFlag(JGUI_Text_Flag_Border);


	m_text = CreateJGUIComponent<JGUIEmptyRectangle>("textrect");
	auto client_rect = GetClientRect();
	m_text->GetTransform()->SetLocalPosition(client_rect.left , client_rect.top);
	m_text->GetTransform()->SetSize(client_rect.width(), client_rect.height());
	m_text->SetThickness(0.5f);
	m_text->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void JGUIForm::Tick(const JGUITickEvent& e)
{
	if (m_Txt)
	{
		m_Txt->SetText(GetName());
	}
}


void JGUIForm::Resize(const JGUIResizeEvent& e)
{
	if (m_Menu)
	{
		m_Menu->GetTransform()->SetSize(e.width, 20);
	}
	if (m_Tab)
	{
		m_Tab->GetTransform()->SetSize(e.width, 20);
	}
	if (m_Title)
	{
		m_Title->GetTransform()->SetSize(e.width, 20);
	}
	if (m_ResizeBox)
	{
		m_ResizeBox->PositionAdjustment();
	}
	if (m_Border)
	{
		m_Border->GetTransform()->SetSize(GetTransform()->GetSize());
	}
	if (m_text)
	{
		auto client_rect = GetClientRect();
		m_text->GetTransform()->SetLocalPosition(client_rect.left, client_rect.top);
		m_text->GetTransform()->SetSize(client_rect.width(), client_rect.height());
	}

}

void JGUIForm::KeyDown(const JGUIKeyDownEvent& e)
{
	if (m_Menu)
	{
		m_Menu->ReceiveKeyEvent(e);
	}


}





void JGUIForm::KeyUp(const JGUIKeyUpEvent& e)
{
	
	if (m_Menu)
	{
		m_Menu->ReceiveKeyEvent(e);
	}
	// TestCode
	if (e.Code == KeyCode::A)
	{
		if (testwindow)
		{
			if (testwindow->IsActive())
			{
				testwindow->SetActive(false);
			}
			else
			{
				testwindow->SetActive(true);
			}
		}

	}

	if (e.Code == KeyCode::Home)
	{

		testwindow = CreateJGUIWindow<JGUIForm>("TestForm"  + to_string(cnt++));
		testwindow->GetTransform()->SetSize(300, 300);
		testwindow->GetTransform()->SetLocalPosition(100, 100);
		//if (m_Tab)
		//{
		//	m_Tab->BindWindow(testwindow);

		//}

	
	}
	if (e.Code == KeyCode::End)
	{
		SetParent(nullptr);
	}
}

void JGUIForm::ProcessByWindowFlags(EJGUI_WindowFlags flag)
{
	// Resize
	if (flag & JGUI_WindowFlag_EnableResize)
	{
		if (m_ResizeBox == nullptr)
		{
			m_ResizeBox = CreateJGUIComponent<JGUIResizeBox>("ResizeBox");
		}

	}
	else
	{
		if (m_ResizeBox)
		{
			DestroyJGUIComponent(m_ResizeBox);
			m_ResizeBox = nullptr;
		}
	}
	// TitleBar
	if (flag & JGUI_WindowFlag_TitileBar)
	{
		if (m_Title == nullptr)
		{
			m_Title = CreateJGUIComponent<JGUITitleBar>("TitleBar");
		}
	}
	else
	{
		if (m_Title)
		{
			DestroyJGUIComponent(m_Title);
			m_Title = nullptr;
		}
	}

	// Menu
	if (flag & JGUI_WindowFlag_MenuBar)
	{
		if (m_Menu == nullptr)
		{
			m_Menu = CreateJGUIComponent<JGUIMenu>("JGUIMenu");
			// Menu
			if (m_Menu)
			{
				// mainmenu 1
				{
					JGUIMenuItemDesc desc;
					desc.name = "MainMenu 1";
					desc.is_use_keyboardkey = true;
					desc.keyboardKey = KeyCode::A;
					auto main_item1 = m_Menu->AddMenuItem(desc);

					// sub_item 1
					{
						JGUIMenuItemDesc desc;
						desc.name = "SubItem 1";
						desc.is_use_keyboardkey = true;
						desc.keyboardKey = KeyCode::C;
						auto sub_item1 = main_item1->AddMenuItem(desc);

						{

							JGUIMenuItemDesc desc;
							desc.name = "SubSubItem 1";
							auto sub_sub_item1 = sub_item1->AddMenuItem(desc);

						}
						{

							JGUIMenuItemDesc desc;
							desc.name = "SubSubItem 2";
							auto sub_sub_item2 = sub_item1->AddMenuItem(desc);

						}
						{

							JGUIMenuItemDesc desc;
							desc.name = "SubSubItem 3";
							auto sub_sub_item3 = sub_item1->AddMenuItem(desc);

						}
					}

					// sub_item 1
					{
						JGUIMenuItemDesc desc;
						desc.name = "SubItem 2";
						desc.is_use_shortcut_key = true;
						desc.is_ctrl = true;
						desc.is_shift = true;
						desc.shortcut_key = KeyCode::D;
						auto sub_item2 = main_item1->AddMenuItem(desc);
					}

					// sub_item 1
					{
						JGUIMenuItemDesc desc;
						desc.name = "SubItem 3";
						desc.is_use_keyboardkey = true;
						desc.keyboardKey = KeyCode::B;
						auto sub_item3 = main_item1->AddMenuItem(desc);

						{

							JGUIMenuItemDesc desc;
							desc.name = "SubSubItem 1";
							desc.is_use_keyboardkey = true;
							desc.keyboardKey = KeyCode::Q;
							auto sub_sub_item1 = sub_item3->AddMenuItem(desc);
							{

								JGUIMenuItemDesc desc;
								desc.is_use_shortcut_key = true;
								desc.is_alt = true;
								desc.shortcut_key = KeyCode::E;
								desc.is_binding_func = true;
								desc.func = [&]()
								{
									auto window = CreateJGUIWindow<JGUIForm>("TestForm" + to_string(cnt++));
									window->GetTransform()->SetLocalPosition(200, 100);
									window->GetTransform()->SetSize(300, 300);
								};
								desc.name = "SubSubSubItem 1";
								auto sub_sub_item2 = sub_sub_item1->AddMenuItem(desc);

							}
							{

								JGUIMenuItemDesc desc;
								desc.name = "SubSubSubItem 2";
								auto sub_sub_item2 = sub_sub_item1->AddMenuItem(desc);

							}
						}
						{

							JGUIMenuItemDesc desc;
							desc.name = "SubSubItem 2";
							auto sub_sub_item2 = sub_item3->AddMenuItem(desc);

						}
						{

							JGUIMenuItemDesc desc;
							desc.name = "SubSubItem 3";
							auto sub_sub_item3 = sub_item3->AddMenuItem(desc);

						}
					}
				}
			}
		}
	}
	else
	{
		if (m_Menu)
		{
			DestroyJGUIComponent(m_Menu);
			m_Menu = nullptr;
		}
	}

	// Tab
	if (flag & JGUI_WindowFlag_TabBar)
	{
		if (m_Tab == nullptr)
		{
			m_Tab = CreateJGUIComponent<JGUITab>("JGUITab");
		}
	}
	else
	{
		if (m_Tab)
		{
			DestroyJGUIComponent(m_Tab);
			m_Tab = nullptr;
		}
	}
	float offset_x = 0.0f;
	float offset_y = 0.0f;

	if (m_Border)
	{
		offset_x = 1.5f;
		offset_y = 1.5f;
	}

	JVector2 window_size = GetTransform()->GetSize();

	if (m_Title)
	{
		m_Title->GetTransform()->SetLocalPosition(offset_x, offset_y);
		m_Title->GetTransform()->SetSize(window_size.x - (2 * offset_x), 20);
		offset_y += 20;
	}
	if (m_Menu)
	{
		m_Menu->GetTransform()->SetLocalPosition(offset_x, offset_y);
		m_Menu->GetTransform()->SetSize(window_size.x - (2 * offset_x), 20);
		offset_y += 20;
	}
	if (m_Tab)
	{
		m_Tab->GetTransform()->SetLocalPosition(offset_x, offset_y);
		m_Tab->GetTransform()->SetSize(window_size.x - (2 * offset_x), 20);
		offset_y += 20;
	}
	if (m_ResizeBox)
	{
		m_ResizeBox->GetTransform()->SetSize(20, 20);
	}
	//






}

JGUIRect JGUIForm::GetClientRect() const
{
	JGUIRect rect = GetPanel()->GetTransform()->GetLocalRect();

	if (m_Border)
	{
		rect.top    += 1.5f;
		rect.left   += 1.6f;
		rect.bottom -= 1.5f;
		rect.right  -= 1.6f;
	}


	if (m_Title)
	{
		auto title_size = m_Title->GetTransform()->GetSize();
		rect.top    += title_size.y;
	}
	if (m_Menu)
	{
		auto menu_size = m_Menu->GetTransform()->GetSize();
		rect.top += menu_size.y;
	}
	if (m_Tab)
	{
		auto tab_size = m_Tab->GetTransform()->GetSize();
		rect.top += tab_size.y;
	}


	return rect;
}





