#pragma once
#include "Class/UI/UIView.h"
#include "Class/UI/UIViewModel/InspectorViewModel.h"


namespace JG
{
	
	class InspectorView : public UIView<InspectorViewModel>
	{
		JGCLASS

		bool mOpenGUI = true;
		bool mIsOpenPopup = false;
		InspectorViewModel* mVm = nullptr;

		char mFindFilterStr[256];
	public:
		InspectorView();
		virtual ~InspectorView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
	private:

	};
}


