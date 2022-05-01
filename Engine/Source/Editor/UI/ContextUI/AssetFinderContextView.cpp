#include "pch.h"
#include "AssetFinderContextView.h"
#include "ExternalImpl/JGImGui.h"
#include "Class/Asset/Asset.h"
#include "Application.h"
namespace JG
{
	const String& AssetFinderContextView::GetSelectedAssetPath() const
	{
		return mSelectedAssetPath;
	}
	void AssetFinderContextView::Initialize(const AssetFinderInitData& data)
	{
		ImGui::OpenPopup(std::to_string(GetViewID()).c_str());
		mFilterFormat = data.AssetFormat;
		UnLock();
	}
	void AssetFinderContextView::Destroy()
	{
		if (mFindAssetThreadSH != nullptr)
		{
			mFindAssetThreadSH->Reset();
			mFindAssetThreadSH = nullptr;
		}

		mFilteredAssetQueue_Thread = Queue<PathData>();
		
		mFilteredAssetList.clear();
		mFilterFormat = EAssetFormat::None;

		UnLock();
	}
	bool AssetFinderContextView::OnGUI()
	{
		if (ImGui::BeginPopupContextWindow(std::to_string(GetViewID()).c_str()) == true)
		{
			char buf[256] = { 0, };
			ImGui::InputText("##Finder Serach", buf, 256);
			ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

			ImGui::BeginChild("##AssetList", ImVec2(0, 300.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoMove);
			FindAsset();

			bool _bool = false;
			for (const PathData& p : mFilteredAssetList)
			{
				if (ImGui::Selectable(p.Name.c_str(), &_bool))
				{
					mSelectedAssetPath = p.Path;
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
	void AssetFinderContextView::FindAsset()
	{
		if (mFindAssetThreadSH == nullptr)
		{
			mFindAssetThreadSH = Scheduler::GetInstance().ScheduleAsync([this]() { FindAsset_Thread(); });
		}





		{
			Lock();
			u32 cnt = 0;
			while (mFilteredAssetQueue_Thread.empty() == false)
			{
				if (cnt >= mMaxReceivePathCount) break;
				mFilteredAssetList.push_back(mFilteredAssetQueue_Thread.front());
				mFilteredAssetQueue_Thread.pop();

				++cnt;
			}
			UnLock();
		}

	}
	void AssetFinderContextView::FindAsset_Thread()
	{
		const String& assetPath = Application::GetAssetPath();
		for (auto& iter : fs::recursive_directory_iterator(assetPath))
		{
			const fs::path& p = iter.path();
			if (p.extension() != JG_ASSET_FORMAT)
			{
				
				continue;
			}
			EAssetFormat format = AssetDataBase::GetInstance().GetAssetFormat(p.string());
			
			if (mFilterFormat == format)
			{
				while (IsLock()) {}

				mFilteredAssetQueue_Thread.push(PathData(p));
			}
		}
	}
}