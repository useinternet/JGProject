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
	class AnimationBlendSpace1D;
	class AnimationBlendSpace1DView : public UIView
	{
		JGCLASS

		const String BLEND_SPACE_NAME = "DefaultBlendSpace";

		struct AnimClipData
		{
			String Name;
			SharedPtr<Asset<AnimationClip>> Asset;
			f32 Value;
		};
		struct AnimationBlendSpace1DBuildData
		{
			String XParamName;
			JVector2 MinMaxValue;
			List <AnimClipData> AnimClipDatas;
		};
	private:
		const JVector2 mSceneResolution = JVector2(350.0f, 450.0f);
		const f32 mInspectorWidth = 490.0f;


		SharedPtr<Graphics::StaticRenderObject> mSkyBox;
		SharedPtr<Graphics::StaticRenderObject> mModel;

		List<SharedPtr<Asset<IMaterial>>> mMaterialAssetList;
		SharedPtr<Asset<Skeletone>> mSkeletoneAsset = nullptr;
		SharedPtr<Asset<IMesh>>		mMeshAsset = nullptr;
		SharedPtr<Asset<AnimationBlendSpace1D>>	mAnimBlendAsset = nullptr;


		UniquePtr<EditorUIScene> mEditorUIScene;

		StorableString mModelAssetPath;
		StorableString mSkeletoneAssetPath;
		StorableString mMaterialAssetPath;

		SharedPtr<AnimationController> mAnimController;

		AnimationBlendSpace1DBuildData mBuildData;
	public:
		AnimationBlendSpace1DView();
		virtual ~AnimationBlendSpace1DView() = default;
	protected:
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override { }
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override { }
	public:
		void SetAnimationBlendSpace1D(const String& blendSpace1DPath);
	private:
		void Build();
		void RefreshAsset(const String& assetPath);
		void LeftOnGUI();
		void RightOnGUI();
		void SetMesh(const String& meshAssetPath);
		void SetSkeletal(const String& skeletalAssetPath);
		void SetMaterial(const List<String>& materialAssetPath);
		void UpdateScene();
	};
}