#include "pch.h"
#include "MessageBoxModalView.h"
#include "Application.h"
namespace JG
{
	void MessageBoxModalView::Initialize(const MessageBoxInitData& data)
	{
		mIsOpenPopup = false;
		mData = data;




		auto padding = ImGui::GetStyle().FramePadding;
		f32 fontSize = ImGui::GetFontSize();
		f32  w = 0.0f;
		f32  msgHeight = 0.0f;
		for (i32 i = 0; i < mData.Contents.length(); ++i)
		{
			String c; c += mData.Contents[i];
			if (c == "\n")
			{
				w = 0.0f;
				msgHeight += fontSize;
				continue;
			}
			auto charSize = ImGui::CalcTextSize(c.c_str());
			w += charSize.x;
			if (w >= mPopupSize.x - padding.x)
			{
				mData.Contents.insert(i, "\n");
				msgHeight += fontSize;
				++i; w = charSize.x;
			}
		}
		msgHeight += mBtAreaHeight;
		mPopupSize.y = std::max<float>(msgHeight, mPopupSize.y);
	}

	void MessageBoxModalView::Destroy()
	{
	}

	bool MessageBoxModalView::OnGUI()
	{

		auto id = mData.Title + "##" + std::to_string(GetType().GetID());
		auto displaySize = ImGui::GetIO().DisplaySize;
		auto appPos = Application::GetInstance().GetWindow()->GetPosition();
		auto winSize = ImVec2(mPopupSize.x, mPopupSize.y);
		f32 fontSize = ImGui::GetFontSize();

		if (mIsOpenPopup == false)
		{
			mIsOpenPopup = true;
			ImGui::OpenPopup(id.c_str());
		}

		ImGui::SetNextWindowSize(winSize);
		ImGui::SetNextWindowPos(ImVec2(appPos.x + displaySize.x * 0.5f - winSize.x * 0.5f, appPos.y + displaySize.y * 0.5f - winSize.y * 0.5f));
		if (ImGui::BeginPopupModal(id.c_str(), nullptr, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{

			ImGui::Text(mData.Contents.c_str());

			auto cursorPos = ImGui::GetCursorPos();
			cursorPos.y = winSize.y - mBtAreaHeight + 10.0F;




			switch (mData.MsgType)
			{
			case EMessageBoxType::ONE_PARAM:
			{
				f32 hw   = winSize.x * 0.5f;
				f32 bthw = mBtWidth * 0.5f;
				cursorPos.x = hw - bthw;

				ImGui::SetCursorPos(cursorPos);
				if (ImGui::Button(mData.ParamName[0].c_str(), ImVec2(mBtWidth, 0.0f)) == true)
				{
					if (mData.ParamAction[0] != nullptr)
					{
						mData.ParamAction[0]();
					}
					Close();
				}
			}
				break;
			case EMessageBoxType::TWO_PARAM:
			{
				f32 hw  = winSize.x * 0.5f;
				f32 hhw = hw * 0.5f;
				f32 bthw = mBtWidth * 0.5f;
				cursorPos.x = hhw - bthw;


				ImGui::SetCursorPos(cursorPos);
				if (ImGui::Button(mData.ParamName[0].c_str(), ImVec2(mBtWidth, 0.0f)) == true)
				{
					if (mData.ParamAction[0] != nullptr)
					{
						mData.ParamAction[0]();
					}
					Close();
				}ImGui::SameLine();

				cursorPos.x = hw + hhw - bthw;
				ImGui::SetCursorPos(cursorPos);
				if (ImGui::Button(mData.ParamName[1].c_str(), ImVec2(mBtWidth, 0.0f)) == true)
				{
					if (mData.ParamAction[1] != nullptr)
					{
						mData.ParamAction[2]();
					}
					Close();
				}
			}

				break;
			}


			ImGui::EndPopup();
			return true;
		}
		else
		{
			return false;
		}
		return false;
	}



}