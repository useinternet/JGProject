#pragma once
#include "JGCore.h"
#include "UI/UIView.h"


namespace JG
{
	class ProjectSettingView : public UIView
	{
		JGCLASS

	private:
		enum
		{
			Category_Start_Setting,
			Category_Input,
			Category_Count,
		};
		enum
		{
			Icon_Add,
			Icon_Close,
			Icon_Count
		};
		struct Category
		{
			bool   IsSelected = false;
			String Label;
			Category() {}
			Category(const String& label) : Label(label) {}
		};
	private:
		bool	 mOpenGUI = true;
		Category mCategoryList[Category_Count];
		Asset<ITexture>* mIcons[Icon_Count];
		SortedSet<String> mKeyNames;




		i32 mCurrentCategory = 0;
	public:
		ProjectSettingView();
		virtual ~ProjectSettingView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent & e) override;
	private:
		void Category_OnGUI();
		void Contents_OnGUI();
	private:
		void StartSetting_OnGUI();
		void Input_OnGUI();
	private:
		bool ActionMappingData_OnGUI(ActionMappingData* data);
		bool AxisMappingData_OnGUI(AxisMappingData* data);
	private:
		void LoadConfig();
		void LoadIcons();
		ImTextureID GetTextureID(i32 iconEnum) const;
	};



}