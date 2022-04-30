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
	class EditorUIScene;
	class Skeletone;
	class AnimationClip;
	class AnimationController;
	class AnimationClipView : public UIView
	{
		JGCLASS

		static constexpr char* CLIP_NAME = "DefaultAnimationClip";
	private:
		SharedPtr<Graphics::StaticRenderObject>   mSkyBox;
		SharedPtr<Graphics::SkeletalRenderObject> mModel;

		List<SharedPtr<Asset<IMaterial>>> mMaterialAssetList;
		SharedPtr<Asset<Skeletone>> mSkeletoneAsset = nullptr;
		SharedPtr<Asset<IMesh>>		mMeshAsset = nullptr;
		SharedPtr<Asset<AnimationClip>>	mAnimClipAsset = nullptr;


		UniquePtr<EditorUIScene> mEditorUIScene;

		StorableString mModelAssetPath;
		StorableString mSkeletoneAssetPath;
		StorableString mMaterialAssetPath;

		UniquePtr<AnimationController> mAnimController;
	public:
		AnimationClipView();
		virtual ~AnimationClipView() = default;

		
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void PreOnGUI() override;
		virtual void LateOnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override { }
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override { }
	public:
		void SetAnimationClip(const String& animClipPath);
	private:
		void LeftOnGUI();
		void RightOnGUI();
		void SetMesh(const String& meshAssetPath);
		void SetSkeletal(const String& skeletalAssetPath);
		void SetMaterial(const List<String>& materialAssetPath);
		void UpdateScene();
	};
}