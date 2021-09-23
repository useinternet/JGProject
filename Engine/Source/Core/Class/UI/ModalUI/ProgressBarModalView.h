#pragma once
#include "Class/UI/UIView.h"



namespace JG
{
	class ProgressBarInitData
	{
	public:
		String Title = "None";
	public:
		ProgressBarInitData() = default;
		ProgressBarInitData(const String& title) : Title(title) {}
	};


	class ProgressBarModalView : public ModalUIView<ProgressBarInitData>
	{
		JGCLASS
	private:
		std::mutex mMutex;
		String mTitle;
		f32    mCurrProgressRatio = 0.0f;
		String mContents;

		bool mIsOpenPopup = false;
	protected:
		virtual void Initialize(const ProgressBarInitData& data) override;
		virtual void Destroy() override;
		virtual bool OnGUI() override;
	public:
		void Display(const String& contents, f32 ratio);
	};
}