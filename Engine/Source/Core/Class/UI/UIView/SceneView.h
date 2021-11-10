#pragma once
#include "JGCore.h"
#include "Class/UI/UIView.h"
#include "Class/Asset/Asset.h"
namespace JG
{
	class GameNode;
	class SceneView : public UIView
	{
		JGCLASS
	private:
		enum
		{
			Icon_TRANSLATE,
			Icon_ROTATION,
			Icon_SCALE,
			Icon_WORLD,
			Icon_LOCAL,
			Icon_PLAY,
			Icon_PAUSE,
			Icon_STOP,
			MAX_ICON,
		};
		enum
		{
			Task_ColumnInit
		};
		enum
		{
			Resolution_FreeAspect,
			Resolution_FHD_1920_1080,
			Resolution_QHD_2560_1440,
			Resolution_Max
		};
		enum
		{
			CameraMode_2D,
			CameraMode_3D,
		};
		enum
		{
			Game_Wait,
			Game_Play,
			Game_Pause,
		};
	private:
		bool mOpenGUI = true;


		GameNode*           mSelectedGameNode = nullptr;
		SharedPtr<ITexture> mSceneTexture;


		i32 mCurrentResolution = Resolution_FHD_1920_1080;

		List<Asset<ITexture>*>        mIcons;
		UniquePtr<Command<GameNode*>> mShowGizmo;


		i32 mCurrentGizmoOperation = 0;
		i32 mCurrentGizmoMode      = 0;
		i32 mCurrentCameraMode     = 0;
		i32 mCurrentGameControll = Game_Wait;
		bool mEnableEditorCameraControll = false;
		bool mIsSnap = false;
		f32 mSnapValue[3] = { 5.0f,10.0f,1.0f };
		JVector2 mMousePos;
		JVector2 mPrevMousePos;

		const ImVec4 mPressedButtonColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
		const ImVec2 mButtonSize = ImVec2(20, 20);

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

		virtual void OnEvent(IEvent& e) override;
	private:
		void OnGUI_Top();
		void OnGUI_Bottom();
		void OnGUI_GizmoTool();
		void OnGui_GameLogicTool();
		void OnGui_FPS();
		void OnGui_ResolutionTool();
	private:
		bool ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e);
		bool ResponseChangeMainSceneTexture(NotifyChangeMainSceneTextureEvent& e);
		bool ResponseDestroyGameObject(NotifyDestroyJGObjectEvent& e);
		bool ResponseChangeGameWorld(NotifyChangeGameWorldEvent& e);
	private:
		void      SetSelectedGameNode(GameNode* gameNode);
		GameNode* GetSelectedGameNode() const;

		void                SetSceneTexture(SharedPtr<ITexture> sceneTexture);
		SharedPtr<ITexture> GetSceneTexture() const;
		ImTextureID GetIconTextureID(i32 iconEnum) const;
		JVector2    GetFitSize(JVector2 originSize, f32 wratio, f32 hratio);
	private:
		String ResolutionToString(i32 resolution);
		void   LoadIcons();
		void   PlayGame();
		void   StopGame();
		void   PauseGame();

	private:
		void ControllEditorCamera();

	};
}