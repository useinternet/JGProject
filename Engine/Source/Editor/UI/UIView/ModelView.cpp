#include "pch.h"
#include "ModelView.h"
#include "UI/UIManager.h"

namespace JG
{
	ModelView::ModelView()
	{
		DisableUniqueView();
	}
	void ModelView::Load()
	{

	}
	void ModelView::Initialize()
	{

	}
	void ModelView::OnGUI()
	{
		ImGui::Text("This is Model View");
	}
	void ModelView::Destroy()
	{
	}
	void ModelView::OnEvent(IEvent& e)
	{
	}
}