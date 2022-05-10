#pragma once
#include "JGCore.h"
#include "UI/UIView.h"
#include "Class/Asset/Asset.h"

namespace JG
{
	namespace Graphics
	{
		class Scene;
		class StaticRenderObject;
		class SkeletalRenderObject;
	}
	namespace NodeGUI
	{
		class NodeEditor;
	}
	namespace StateNodeGUI
	{
		class StateNodeEditor;
	}
	class EditorUIScene;

	class AnimationView : public UIView
	{
		JGCLASS
	private:
		SharedPtr<Graphics::StaticRenderObject>   mSkyBox;
		SharedPtr<Graphics::SkeletalRenderObject> mModel;

		UniquePtr<EditorUIScene> mEditorUIScene;
		UniquePtr<StateNodeGUI::StateNodeEditor> mNodeEditor;
		StorableString mModelAssetPath;
	public:
		AnimationView();
		virtual ~AnimationView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override { }
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override { }
	private:
		void CreateRootNode();
		void CreateAnimationClipNode();

		void UpdateScene();
	};
}