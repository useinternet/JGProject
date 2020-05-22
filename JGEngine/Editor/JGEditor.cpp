#include "pch.h"
#include "JGEditor.h"
#include "App/Application.h"
#include "Window/Debug/JGLog.h"
#include "Window/Debug/JGUIHierarchy.h"

using namespace std;
void JGEditor::Awake()
{
	GlobalLinkData::Init(Application::GetLinkStream());


}

void JGEditor::Start()
{

	InitMainMenu();

	m_FPS = CreateJGUIElement("FPS")->CreateJGUIComponent<Text>();
	m_FPS->SetFontSize(32.0f);
	m_FPS->GetTransform()->SetLocalPosition(50, 50);
}

void JGEditor::Tick(const JGUITickEvent& e)
{
	m_FPS->SetText(to_string(e.fps));
}

void JGEditor::InitMainMenu()
{
	MainMenu* mainMenu = GetMainMenu();

	if (mainMenu == nullptr) return;



	// file
	{
		MainMenuItemDesc fileItemDesc;
		fileItemDesc.contents      = "File";
		fileItemDesc.name          = "File";
		fileItemDesc.auxiliary_key = KeyCode::F;


		MainMenuItem* file = mainMenu->AddItem(fileItemDesc);

		// Open
		{
			MenuItemDesc openItemDesc;
			openItemDesc.contents      = "Open";
			openItemDesc.name          = "Open";
			openItemDesc.auxiliary_key = KeyCode::O;


			MenuItem* open = file->AddItem(openItemDesc);
		}




	}
	// window
	{
		MainMenuItemDesc windowItemDesc;
		windowItemDesc.contents      = "Window";
		windowItemDesc.name          = "Window";
		windowItemDesc.auxiliary_key = KeyCode::W;


		MainMenuItem* window = mainMenu->AddItem(windowItemDesc);
		

		// Debug
		{
			MenuItemDesc debugItemDesc;
			debugItemDesc.contents      = "Debug";
			debugItemDesc.name          = "Debug";
			debugItemDesc.auxiliary_key = KeyCode::D;


			MenuItem* debug = window->AddItem(debugItemDesc);


			// Log
			{
				MenuItemDesc logItemDesc;
				logItemDesc.contents      = "Log";
				logItemDesc.name          = "Log";
				logItemDesc.auxiliary_key = KeyCode::L;
				logItemDesc.func = [&](void* data)
				{
					JGUIForm* logForm = CreateJGUIForm<JGLog>("Log");
					logForm->GetTransform()->SetLocalPosition(50, 50);



				};
				MenuItem* log = debug->AddItem(logItemDesc);
			}

			// JGUIHierarchicalStructure
			{
				MenuItemDesc jhsItemDesc;
				jhsItemDesc.contents = "JGUIHierarchy";
				jhsItemDesc.name = "JGUIHierarchy";
				jhsItemDesc.auxiliary_key = KeyCode::H;
				jhsItemDesc.func = [&](void* data)
				{
					JGUIForm* jhsForm = CreateJGUIForm<JGUIHierarchy>("JGUIHierarchy");
					jhsForm->GetTransform()->SetLocalPosition(50, 50);



				};
				MenuItem* jhs = debug->AddItem(jhsItemDesc);
			}
		}





	}
}
