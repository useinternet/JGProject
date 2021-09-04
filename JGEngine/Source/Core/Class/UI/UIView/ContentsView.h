#pragma once


#include "Class/UI/UIView.h"
#include "Class/UI/UIViewModel/ContentsViewModel.h"

namespace JG
{
	class ContentsView : public UIView<ContentsViewModel>
	{
		JGCLASS
		class Directory {};
	private:
		enum
		{
			ICON_NONE,
			ICON_DIRECTORY,
			ICON_MATERIAL,
			ICON_GAMEWORLD,
			ICON_TEXTURE,
			ICON_MESH,
			ICON_IMPORT,
			MAX_ICON
		};

	private:
		List<Asset<ITexture>*> mIcons;

		ContentsViewModel* mVm = nullptr;
	private:
		bool mOpenGUI = true;
		bool mIsColumnInit = true;
		bool mIsCtrl = false;
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
	private:
		void OnGui_TopBar();
		void OnGui_ImportDialog();
	private:
		void LoadIcons();
		i32  GetIconID(EAssetFormat format);
		ImTextureID GetIconTextureID(i32 iconEnum);
	};
}