#include "pch.h"
#include "UIViewModel.h"
#include "UIModel.h"
#include "UIView.h"
namespace JG
{
	void UIViewModel::Destroy()
	{
		for (auto& _pair : mUIModelPool)
		{
			_pair.second->Destroy();
		}
	}
	void UIViewModel::OnEvent(IEvent& e)
	{
	}
	void UIViewModel::SendError(SharedPtr<IUIError> error)
	{
		mUIErrorReceiver->ReceiveError(error);
	}
	void UIViewModel::SetErrorReceiver(IUIErrorReceiver* receiver)
	{
		mUIErrorReceiver = receiver;
	}
}