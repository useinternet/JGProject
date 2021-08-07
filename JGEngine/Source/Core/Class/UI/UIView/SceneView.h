#pragma once
#include "JGCore.h"
#include "Class/UI/UIView.h"
#include "Class/UI/UIViewModel/SceneViewModel.h"

namespace JG
{
	class SceneView : public UIView<SceneViewModel>
	{
		JGCLASS
		bool mOpenGUI = true;


		i32 mCurrentGizmoOperation = 0;
		i32 mCurrentGizmoMode = 0;
		i32 mCurrentCameraMode = 0;


		bool mEnableEditorCameraControll = false;
		bool mIsSnap = true;
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
		void ControllEditorCamera();
	};
}