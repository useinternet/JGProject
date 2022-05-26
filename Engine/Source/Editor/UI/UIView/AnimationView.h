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
	// Layer ��� �ִϸ��̼�
	// Animation ���� Base Layer, SubLayer�� �ְ�
	// Base Layer������ AnimClip, BlendSpace, BlendSpace1D ��� ��� ����
	// Sub  Layer������ AnimClip�� ���
	// Sub Layer�� ��쿡�� RootNode�� ���Ҿ� Begin, Exit ��尡 ����
	// Exit ��忡 �����ϸ� Layer ��� �Ϸ� RootNode���� �ٽ� ����
	// �����Ϳ����� SubLayer <-> Base Layer ��ȯ ����



	class AnimationClip;
	class AnimationBlendSpace1D;
	class AnimationBlendSpace;
	class Skeletone;
	class EditorUIScene;
	class IMesh;
	
	enum class ENodeType;


	class AnimationView : public UIView
	{
		JGCLASS
	private:
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
		//UniquePtr<StateNodeGUI::StateNodeEditor> mNodeEditor;


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
		List<struct AnimParamBuildData>	  mAnimParamBuildDataList;




		Dictionary<String, SharedPtr<struct AnimationLayerBuildData>> mAnimLayerBuildDataDic;
		SharedPtr<AnimationLayerBuildData> mCurrentAnimLayerBuildData = nullptr;



		//Dictionary<StateNodeGUI::StateNodeID, AnimTransitionBuildData> mAnimTransitionBuildDataDic;
		//Dictionary<StateNodeGUI::StateNodeID, AnimClipBuildData>       mAnimClipBuildDataDic;
		//Dictionary<StateNodeGUI::StateNodeID, AnimBlendSpace1DBuildData>    mAnimBlendSpace1DBuildDataDic;
		//Dictionary<StateNodeGUI::StateNodeID, AnimBlendSpaceBuildData>      mAnimBlendSpaceBuildDataDic;
		//Dictionary<StateNodeGUI::StateNodeID, JVector2> mNodeLocationDic;
		//Dictionary<String, StateNodeGUI::StateNodeID>   mNodeNameDic;




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

		void AnimationScene_OnGUI();
		void AnimationInspector_OnGUI();
		void AnimationNodeEditor_OnGUI();
		void AnimationParamEditor_OnGUI();

		void AnimationParam_OnGUI(struct AnimParamBuildData& buildData);

		void Transition_OnGUI(struct AnimTransitionBuildData& buildData);
		void TransitionConditionValue_OnGUI(struct AnimTransitionConditionBuildData& buildData);


		void AnimationRoot_OnGUI();
		void AnimationClip_OnGUI(struct AnimClipBuildData& buildData);
		void AnimationBlendSpace1D_OnGUI(struct AnimBlendSpace1DBuildData& buildData);
		void AnimationBlendSpace_OnGUI(struct AnimBlendSpaceBuildData& buildData);


		StateNodeGUI::StateNodeID CreateNode(const String& layerName, ENodeType nodeType, const String& name, const JVector2& initPos = JVector2(JG_F32_MAX, JG_F32_MAX));
		
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

		SharedPtr<AnimationLayerBuildData> GetCurrentLayerBuildData() const;
		SharedPtr<AnimationLayerBuildData> GetLayerBuildData(const String& name) const;
	};
}