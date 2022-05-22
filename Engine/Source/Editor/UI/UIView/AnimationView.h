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
	// Layer 기반 애니메이션
	// Animation 에는 Base Layer, SubLayer가 있고
	// Base Layer에서는 AnimClip, BlendSpace, BlendSpace1D 모두 사용 가능
	// Sub  Layer에서는 AnimClip만 허용
	// Sub Layer의 경우에는 RootNode와 더불어 Begin, Exit 노드가 존재
	// Exit 노드에 도달하면 Layer 재생 완료 RootNode부터 다시 시작
	// 에디터에서는 SubLayer <-> Base Layer 전환 가능



	class AnimationClip;
	class AnimationBlendSpace1D;
	class AnimationBlendSpace;
	class Skeletone;
	class EditorUIScene;
	class IMesh;
	

	class AnimationView : public UIView
	{
		JGCLASS
	private:
		enum class ENodeType
		{
			Root,
			AnimationClip,
			AnimationBlendSpace1D,
			AnimationBlendSpace,
		};
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
			f32 TransitionDuration = 0.0f;

			bool HasExitTime = false;
			f32  ExitTime = 0.0f;
			List<AnimTransitionConditionBuildData> Conditions;
		};
		struct AnimClipBuildData
		{
			StateNodeGUI::StateNodeID ID;
			SharedPtr<Asset<AnimationClip>> Asset;
		
			EAnimationClipFlags Flags = EAnimationClipFlags::None;
			f32 Speed    = 1.0f;
		};
		struct AnimBlendSpace1DBuildData
		{
			StateNodeGUI::StateNodeID ID;
			EAnimationBlendSpace1DFlag Flags = EAnimationBlendSpace1DFlag::Repeat;
			SharedPtr<Asset<AnimationBlendSpace1D>> Asset;
		};
		struct AnimBlendSpaceBuildData
		{
			StateNodeGUI::StateNodeID ID;
			EAnimationBlendSpaceFlag Flags = EAnimationBlendSpaceFlag::Repeat;
			SharedPtr<Asset<AnimationBlendSpace>> Asset;
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

		SharedPtr<Asset<AnimationController>>    mAnimationAsset;
		UniquePtr<StateNodeGUI::StateNodeEditor> mNodeEditor;


		EEditMode  mAnimParamEditMode		    = EEditMode::Default;
		EEditMode  mTransitionConditionEditMode = EEditMode::Default;
		EAnimState mAnimState				    = EAnimState::Editable;

		StorableString mModelAssetPath;
		StorableString mSkeletoneAssetPath;
		StorableString mMaterialAssetPath;
		StorableString mNodeSaveData;

		// BuildData
		Dictionary<EAnimationParameterType, Color> mBgColorByParamTypeDic;
		HashSet<String> mAddedAnimParamNameSet;
		List<AnimParamBuildData>	  mAnimParamBuildDataList;



		// 레이어 기반
		//struct AnimationLayerBuildData
		//{
		//	UniquePtr<StateNodeGUI::StateNodeEditor> NodeEditor;
		//	Dictionary<StateNodeGUI::StateNodeID, AnimTransitionBuildData> AnimTransitionBuildDataDic;
		//	Dictionary<StateNodeGUI::StateNodeID, AnimClipBuildData>       AnimClipBuildDataDic;
		//	Dictionary<StateNodeGUI::StateNodeID, AnimBlendSpace1DBuildData>    AnimBlendSpace1DBuildDataDic;
		//	Dictionary<StateNodeGUI::StateNodeID, AnimBlendSpaceBuildData>      AnimBlendSpaceBuildDataDic;
		//	Dictionary<StateNodeGUI::StateNodeID, JVector2> NodeLocationDic;
		//	Dictionary<String, StateNodeGUI::StateNodeID>   NodeNameDic;
		//};
		//Dictionary<String, AnimationLayerBuildData> mAnimLayerBuildDataDic;




		Dictionary<StateNodeGUI::StateNodeID, AnimTransitionBuildData> mAnimTransitionBuildDataDic;
		Dictionary<StateNodeGUI::StateNodeID, AnimClipBuildData>       mAnimClipBuildDataDic;
		Dictionary<StateNodeGUI::StateNodeID, AnimBlendSpace1DBuildData>    mAnimBlendSpace1DBuildDataDic;
		Dictionary<StateNodeGUI::StateNodeID, AnimBlendSpaceBuildData>      mAnimBlendSpaceBuildDataDic;
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
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override { }
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override { }
	private:
		void InitNodeEditor();
		void InitBuildData();


		void AnimationScene_OnGUI();
		void AnimationInspector_OnGUI();
		void AnimationNodeEditor_OnGUI();
		void AnimationParam_OnGUI(AnimParamBuildData& buildData);

		void Transition_OnGUI(AnimTransitionBuildData& buildData);
		void TransitionConditionValue_OnGUI(AnimTransitionConditionBuildData& buildData);


		void AnimationRoot_OnGUI();
		void AnimationClip_OnGUI(AnimClipBuildData& buildData);
		void AnimationBlendSpace1D_OnGUI(AnimBlendSpace1DBuildData& buildData);
		void AnimationBlendSpace_OnGUI(AnimBlendSpaceBuildData& buildData);


		StateNodeGUI::StateNodeID CreateNode(ENodeType nodeType, const String& name, const JVector2& initPos = JVector2(JG_F32_MAX, JG_F32_MAX));
		StateNodeGUI::StateNodeID CreateRootNode();
		StateNodeGUI::StateNodeID CreateAnimationClipNode(const String& name, const JVector2& initPos = JVector2(JG_F32_MAX, JG_F32_MAX));
		StateNodeGUI::StateNodeID CreateAnimationBlendSpace1DNode(const String& name, const JVector2& initPos = JVector2(JG_F32_MAX, JG_F32_MAX));
		StateNodeGUI::StateNodeID CreateAnimationBlendSpaceNode(const String& name, const JVector2& initPos = JVector2(JG_F32_MAX, JG_F32_MAX));
		void UpdateScene();

		void SetMesh(const String& meshAssetPath);
		void SetSkeletal(const String& skeletalAssetPath);
		void SetMaterial(const List<String>& materialAssetPath);

		void UpdateTransitionBuildData(const String& paramName, EAnimationParameterType type);
		void UpdateTransitionBuildData(const String& oldName, const String& newName);
		void RemoveTransitionBuildData(const String& removedName);

		bool Build();
		void RefreshAnimAsset(const String& path);
		void Play();
		void UpdateFlow();
		void Editable();
		bool IsEditable() const {
			return mAnimState != EAnimState::Playing;
		}
		void SaveNodeLocation();
		Color GetBgColor(EAnimationParameterType type) {
			return mBgColorByParamTypeDic[type];
		}
	};
}