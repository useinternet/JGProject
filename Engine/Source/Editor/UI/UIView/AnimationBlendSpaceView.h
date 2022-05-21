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
	class AnimationBlendSpace;
	class AnimationBlendSpaceView : public UIView
	{
		JGCLASS

		const String BLEND_SPACE_NAME = "DefaultBlendSpace";

		struct AnimClipData
		{
			String Name;
			SharedPtr<Asset<AnimationClip>> Asset;
			JVector2 Value;
			f32 Speed = 1.0f;
		};
		struct AnimationBlendSpaceBuildData
		{
			String XParamName;
			String YParamName;
			JVector2 MinMaxXValue;
			JVector2 MinMaxYValue;
			JVector2Int ValueXYNumGrid;
			Dictionary<u32, AnimClipData> AnimClipDatas;
		};
	private:
		const JVector2 mSceneResolution = JVector2(350.0f, 450.0f);
		const f32 mInspectorWidth = 490.0f;
		const f32 mCoordinateResidualSpace = 30.0F;
		const f32 mDotSize = 13;
		const f32 mDotRounding = 3.0f;
		const i32 mMaxGridCount = 10000;

		SharedPtr<Graphics::StaticRenderObject> mSkyBox;
		SharedPtr<Graphics::StaticRenderObject> mModel;

		List<SharedPtr<Asset<IMaterial>>> mMaterialAssetList;
		SharedPtr<Asset<Skeletone>> mSkeletoneAsset = nullptr;
		SharedPtr<Asset<IMesh>>		mMeshAsset = nullptr;
		SharedPtr<Asset<AnimationBlendSpace>>	mAnimBlendAsset = nullptr;


		UniquePtr<EditorUIScene> mEditorUIScene;

		StorableString mModelAssetPath;
		StorableString mSkeletoneAssetPath;
		StorableString mMaterialAssetPath;
		
		SharedPtr<AnimationController> mAnimController;

		AnimationBlendSpaceBuildData mBuildData;

		JVector2 mSampleValue;
		i32 mSelectedGridKey = -1;
	public:
		AnimationBlendSpaceView();
		virtual ~AnimationBlendSpaceView() = default;
	protected:
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override { }
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override { }
	public:
		void SetAnimationBlendSpace(const String& blendSpacePath);
	private:
		void Build();
		void RefreshAsset(const String& assetPath);
		void LeftOnGUI();
		void RightOnGUI();
		void ValueParamOnGUI();
		void CoordinateOnGUI();
		JVector2 SampleValueDotOnGUI(
			const JVector2& localRectMin, const JVector2& rectMin, const JVector2& rectMax, const JVector2& value);
		void SetMesh(const String& meshAssetPath);
		void SetSkeletal(const String& skeletalAssetPath);
		void SetMaterial(const List<String>& materialAssetPath);
		void UpdateScene();
		void UpdateValueParam();
		u32 GetGridKey(const JVector2Int& gridPos) const {
			return gridPos.y + gridPos.x * mMaxGridCount;
		}
		JVector2Int GetGridPos(u32 key) {
			i32 xGrid = key / mMaxGridCount;
			i32 yGrid = key - xGrid * mMaxGridCount;
			return JVector2Int(xGrid, yGrid);
		}
	};
}