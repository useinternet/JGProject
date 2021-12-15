#include "pch.h"
#include "DevelopView.h"
#include "Application.h"
#include "UI/UIManager.h"
namespace JG
{



	DevelopView::DevelopView()
	{
		UIManager::GetInstance().RegisterMainMenuItem("Windows/Devlop", 0, [&]()
		{
			Open();
		}, nullptr);
	}

	void DevelopView::Load()
	{
	}

	void DevelopView::Initialize()
	{
	}

	void DevelopView::OnGUI()
	{
		ImGui::Begin("Develop", &mOpenGUI);
		Statistics_OnGUI();
		Dev_OnGUI();
		ImGui::End();
		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}

	void DevelopView::Destroy()
	{
	}

	void DevelopView::OnEvent(IEvent& e)
	{
	}

	void DevelopView::Statistics_OnGUI()
	{
		ImGui::BeginChild("Statistics");


		int fps = Application::GetInstance().GetAppTimer()->GetFPS();
		ImGui::Text("FPS : %d", fps);



		ImGui::EndChild();
	}

	void DevelopView::Dev_OnGUI()
	{
		ImGui::BeginChild("Dev Controller");

		ImGui::EndChild();
	}

}