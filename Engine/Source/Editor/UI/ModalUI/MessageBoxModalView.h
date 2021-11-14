#pragma once
#include "UI/UIView.h"



namespace JG
{
	enum class EMessageBoxType
	{
		ONE_PARAM,
		TWO_PARAM
	};
	class MessageBoxInitData
	{

	public:
		String Title = "None";
		String Contents;
		EMessageBoxType MsgType = EMessageBoxType::ONE_PARAM;
		String ParamName[2] = { "OK", "Cancel"};
		std::function<void()> ParamAction[2] = { nullptr, };
	public:
		MessageBoxInitData() = default;
		MessageBoxInitData(const String& title, const String& contents, EMessageBoxType type = EMessageBoxType::ONE_PARAM) : Title(title), Contents(contents), MsgType(type) {}
	};


	class MessageBoxModalView : public ModalUIView<MessageBoxInitData>
	{
		JGCLASS
	private:

		MessageBoxInitData mData;
		JVector2 mPopupSize = JVector2(500, 300);
		f32 mBtAreaHeight = 50.0f;
		f32 mBtWidth = 100.0f;
		bool mIsOpenPopup = false;
	public:
		virtual ~MessageBoxModalView() = default;
	protected:
		virtual void Initialize(const MessageBoxInitData& data) override;
		virtual void Destroy() override;
		virtual bool OnGUI() override;
	};
}