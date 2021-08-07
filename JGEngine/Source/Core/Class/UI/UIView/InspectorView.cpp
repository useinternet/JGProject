#include "pch.h"
#include "InspectorView.h"
#include "Imgui/imgui.h"
#include "Class/Game/GameObjectFactory.h"
#include "Class/Game/GameNode.h"
#include "Class/UI/UIModel/InspectorModel.h"
#include "Class/UI/ContextUI/ComponentFinderContextView.h"

namespace JG
{
	InspectorView::InspectorView()
	{
		UIManager::GetInstance().RegisterMainMenuItem("Windows/Inspector", 0, [&]()
		{
			Open();
		}, nullptr);
		memset(mFindFilterStr, 0, 256);
	}
	void InspectorView::Load()
	{

	}
	void InspectorView::Initialize()
	{
		mVm = GetViewModel();
		memset(mFindFilterStr, 0, 256);
	}
	void InspectorView::OnGUI()
	{
		ImGui::Begin("Inspector", &mOpenGUI);
		if (mVm != nullptr)
		{
			IJGObject* object = mVm->GetTargetObject();
			if (object != nullptr)
			{
				object->OnInspectorGUI();
			}

		}

		ImGui::End();
		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}
	void InspectorView::Destroy()
	{
	}
}