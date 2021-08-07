#pragma once


#include "Class/UI/UIView.h"
#include "Class/UI/UIViewModel/ContentsViewModel.h"

namespace JG
{
	class ContentsView : public UIView<ContentsViewModel>
	{
		JGCLASS
		class Directory {};


		ContentsViewModel* mVm = nullptr;
	private:
		bool mOpenGUI = true;
		bool mIsColumnInit = true;
	public:
		ContentsView();
		virtual ~ContentsView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
	private:
		void OnGui_ContentsDirectory();
		void OnGui_ContentsDirectoryDetail();
	};
}