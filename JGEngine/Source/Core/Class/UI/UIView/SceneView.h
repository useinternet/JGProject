#pragma once
#include "JGCore.h"
#include "Class/UI/UIView.h"
#include "Class/UI/UIViewModel/SceneViewModel.h"
#include "Class/Asset/Asset.h"
namespace JG
{
	class SceneView : public UIView<SceneViewModel>
	{
		JGCLASS
			
	private:
		enum
		{
			Icon_Move,
			Icon_Rotation,
			Icon_Scale,
			Icon_Max,
		};
		Asset<ITexture>* mIcons[Icon_Max];
		u64 mIconIDs[Icon_Max];
	private:
		bool mOpenGUI = true;


		i32 mCurrentGizmoOperation = 0;
		i32 mCurrentGizmoMode = 0;
		i32 mCurrentCameraMode = 0;


		bool mEnableEditorCameraControll = false;
		bool mIsSnap = false;
		f32 mSnapValue[3] = { 5.0f,10.0f,1.0f };
		//
		JVector2 mMousePos;
		JVector2 mPrevMousePos;

	public:
		SceneView();
		virtual ~SceneView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;


		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	private:
		void OnGUI_ToolBar();
	private:
		void ControllEditorCamera();
	};
}