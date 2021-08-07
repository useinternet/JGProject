#pragma once


#include "Class/UI/UIView.h"
#include "Class/UI/UIViewModel/WorldHierarchyViewModel.h"

namespace JG
{

	class WorldHierarchyView : public UIView<WorldHierarchyViewModel>
	{
		JGCLASS

		bool mOpenGUI = true;
		WorldHierarchyViewModel* mVm = nullptr;
	public:
		WorldHierarchyView();
		virtual ~WorldHierarchyView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;

	};

}