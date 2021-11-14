#include "pch.h"
#include "ComponentFinderContextView.h"
#include "GameObjectFactory.h"
namespace JG
{

	void ComponentFinderContextView::Initialize(const ComponentFinderInitData& data)
	{
		ImGui::OpenPopup(std::to_string(GetType().GetID()).c_str());
		mSelectedComponentType.clear();
	}
	void ComponentFinderContextView::Destroy()
	{
		mFilterStr.clear();
		mComponentTypeSet.clear();
		mFilteredTypeList.clear();
		mFilterStr.resize(256);
	}
	bool ComponentFinderContextView::OnGUI()
	{
		if (ImGui::BeginPopupContextWindow(std::to_string(GetType().GetID()).c_str()) == true)
		{
			char buf[256] = { 0, };
			ImGui::InputText("##Finder Serach", buf, 256);
			ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

			ImGui::BeginChild("##ComponentList", ImVec2(0, 600.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoMove);

			FindComponentTypeList(buf);
			bool _bool = false;
			for (auto& type : mFilteredTypeList)
			{
				auto name = ReplaceAll(type, "JG::", "");
				if (ImGui::Selectable(name.c_str(), &_bool))
				{
					mSelectedComponentType = type;
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndChild();
			ImGui::EndPopup();
			return true;
		}
		else
		{
			return false;
		}
	}
	const String& ComponentFinderContextView::GetSelectedComponent() const
	{
		return mSelectedComponentType;
	}
	void ComponentFinderContextView::FindComponentTypeList(const String& filter)
	{
		bool isDirty = false;
		auto& originSet = GameObjectFactory::GetInstance().GetGameComponentTypeSet();
		isDirty |= originSet.size() != mComponentTypeSet.size();
		isDirty |= mFilterStr != filter;

		if (isDirty)
		{
			mFilteredTypeList.clear();
			mComponentTypeSet = originSet;
			mFilterStr = filter;
			for (auto& type : originSet)
			{
				if (mFilterStr.length() == 0)
				{
					mFilteredTypeList.insert(type.GetName());
				}
				auto originName = ReplaceAll(type.GetName(), "JG::", "");
				auto filterName = filter;

				u64 pos = originName.find(filterName);
				if (pos != String::npos)
				{
					mFilteredTypeList.insert(type.GetName());
				}
			}
		}

	}
}