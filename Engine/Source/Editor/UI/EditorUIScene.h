#pragma once
#include "JGCore.h"

namespace JG
{
	class ITexture;
	namespace Graphics
	{
		class Scene;
		class StaticRenderObject;
		class SkeletalRenderObject;
		class SceneObject;
		class Light;
	}

	ENUM_FLAG(EEditorUISceneFlags)
	enum class EEditorUISceneFlags
	{
		None = 0,
		No_MoveRotate = 0x001,
	};

	struct EditorUISceneConfig
	{
		JVector2 Resolution;
		JVector2 ImageSize;
		JVector3 OffsetScale;

		SharedPtr<Graphics::SceneObject> Model;
		SharedPtr<Graphics::SceneObject> SkyBox;

		f32 FarZ  = 1000000.0f;
		f32 NearZ = 1.0f;
		JVector3  EyePos = JVector3(0, 0, -200.0f);
		EEditorUISceneFlags Flags = EEditorUISceneFlags::None;
	};


	class EditorUIScene
	{
		SharedPtr<ITexture> mSceneTexture;
		Graphics::Scene*	mScene = nullptr;
		EditorUISceneConfig mConfig;
		JVector2 mMousePosition;
		JVector2 mPrevMousePosition;

		SharedPtr<Graphics::Light> mLight;

		JVector3 mModelRotation;
	public:
		EditorUIScene(const EditorUISceneConfig& config);
		~EditorUIScene();
		void OnGUI();
		void SetModel(SharedPtr<Graphics::SceneObject> model);
		Graphics::Scene* GetScene() const {
			return mScene;
		}
	private:
		void Init();
		void UpdateScene();
		void UpdateModel();
		void UpdateLight();
		void PushRenderObject();
		void Rendering();
	};
}