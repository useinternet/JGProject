#pragma once
#include "JGCore.h"
#include "UI/UIView.h"
#include "Class/Asset/Asset.h"
#include "Animation/AnimationDefines.h"
namespace JG
{
	namespace Graphics
	{
		class Scene;
		class StaticRenderObject;
	}
	namespace NodeGUI
	{
		class NodeEditor;
	}
	namespace StateNodeGUI
	{
		class StateNodeEditor;
		using StateNodeID = u64;
	}
	class AnimationClip;
	class Skeletone;
	class EditorUIScene;
	class IMesh;

	class AnimationView : public UIView
	{
		JGCLASS
	private:
		struct AnimParamBuildData
		{
			String Name;
			EAnimationParameterType Type;
			List<jbyte> Data;
		};

		struct AnimTransitionConditionBuildData
		{
			String ParamName;
			EAnimationParameterType Type;
			EAnimationCondition Condition;
			List<jbyte> Data;
		};
		struct AnimTransitionBuildData
		{
			StateNodeGUI::StateNodeID From;
			StateNodeGUI::StateNodeID To;
			List<AnimTransitionConditionBuildData> Conditions;
		};
		struct AnimClipBuildData
		{
			StateNodeGUI::StateNodeID ID;
			EAnimationClipFlags Flags = EAnimationClipFlags::None;
			SharedPtr<Asset<AnimationClip>> Asset;
		};
		enum class EEditMode
		{
			Default,
			Delete,
			Move,
		};
		enum class EAnimState
		{
			Editable,
			Playing,
		};

		const f32 mLeftWidth  = 350.0f;
		const f32 mRightWidth = 400.0f;
		const f32 mTableTypeRowWidth = 80.0f;
		const f32 mTableNameRowWidth = 200.0f;
		const f32 mEditRowWidth = 70.0f;

		const f32 mTransitionParamNameRowWidth = 150.0f;
		const f32 mTransitionConditionRowWidth = 150.0f;
		const f32 mTransitionValueRowWidth = 70.0f;
		const JVector2 mSceneResolution = JVector2(mLeftWidth, mLeftWidth + 50.0f);

	private:
		SharedPtr<Graphics::StaticRenderObject> mSkyBox;
		SharedPtr<Graphics::StaticRenderObject> mModel;
		List<SharedPtr<Asset<IMaterial>>> mMaterialAssetList;
		SharedPtr<Asset<Skeletone>> mSkeletoneAsset = nullptr;
		SharedPtr<Asset<IMesh>>		mMeshAsset = nullptr;
		UniquePtr<EditorUIScene>    mEditorUIScene;

		SharedPtr<Asset<AnimationController>> mAnimationAsset;
		UniquePtr<StateNodeGUI::StateNodeEditor> mNodeEditor;


		EEditMode mAnimParamEditMode = EEditMode::Default;
		EEditMode mTransitionConditionEditMode = EEditMode::Default;
		EAnimState mAnimState = EAnimState::Editable;

		StorableString mModelAssetPath;
		StorableString mSkeletoneAssetPath;
		StorableString mMaterialAssetPath;
		StorableString mNodeSaveData;

		// BuildData
		Dictionary<EAnimationParameterType, Color> mBgColorByParamTypeDic;
		HashSet<String> mAddedAnimParamNameSet;
		List<AnimParamBuildData>	  mAnimParamBuildDataList;
		Dictionary<StateNodeGUI::StateNodeID, AnimTransitionBuildData> mAnimTransitionBuildDataDic;
		Dictionary<StateNodeGUI::StateNodeID, AnimClipBuildData>       mAnimClipBuildDataDic;
		Dictionary<StateNodeGUI::StateNodeID, JVector2> mNodeLocationDic;
		Dictionary<String, StateNodeGUI::StateNodeID>   mNodeNameDic;
	public:
		AnimationView();
		virtual ~AnimationView() = default;

		void SetAnimation(const String& animatinoAssetPath);
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override { }
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override { }
	private:
		void AnimationScene_OnGUI();
		void AnimationInspector_OnGUI();
		void AnimationNodeEditor_OnGUI();
		void AnimationParam_OnGUI(AnimParamBuildData& buildData);

		void Transition_OnGUI(AnimTransitionBuildData& buildData);
		void TransitionConditionValue_OnGUI(AnimTransitionConditionBuildData& buildData);
		void AnimationClip_OnGUI(AnimClipBuildData& buildData);
		void CreateRootNode();
		void CreateAnimationClipNode();

		void UpdateScene();

		void SetMesh(const String& meshAssetPath);
		void SetSkeletal(const String& skeletalAssetPath);
		void SetMaterial(const List<String>& materialAssetPath);

		void UpdateTransitionBuildData(const String& paramName, EAnimationParameterType type);
		void UpdateTransitionBuildData(const String& oldName, const String& newName);
		void RemoveTransitionBuildData(const String& removedName);

		// Animation Controller 생성
		// 생성 이후 기존 Controller 는 제거
		bool Build();
		
		void RefreshAnimAsset(const String& path);

		// 플레이 시 노드 에디터 락(편집 불가)  애니메이션 파라미터에 따라서 노드  Flow 가 발생
		void Play();
		void UpdateFlow();
		// 편집 가능,
		void Editable();
		void SaveNodeLocation();
		Color GetBgColor(EAnimationParameterType type) {
			return mBgColorByParamTypeDic[type];
		}
	};
}