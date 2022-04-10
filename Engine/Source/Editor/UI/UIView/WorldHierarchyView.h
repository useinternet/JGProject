#pragma once


#include "UI/UIView.h"

namespace JG
{

	class GameWorld;
	class WorldHierarchyView : public UIView
	{
		JGCLASS


		enum
		{
			NodeState_Selected = 0x0001,
		};
		enum
		{
			PostProcess_None,
			PostProcess_NodeMoveUp,
			PostProcess_NodeMoveDown,
		};
		class IPostProcessData : public IJGObject
		{
			JGCLASS
		public:
			i32 Command = PostProcess_None;
		};
		class PP_MoveInParentData : public IPostProcessData
		{
			JGCLASS
		public:
			GameNode* GameNode = nullptr;
		};

		struct NodeState
		{
			i32 State = 0;
		public:
			bool IsState(i32 state) {
				return State & state;
			}
			void On(i32 state) {
				State |= state;
			}
			void Off(i32 state) {
				State &= ~state;
			}
		};

		bool mIsCtrl = false;




		GameWorld* mGameWorld = nullptr;

		HashSet<GameNode*> mSelectedGameNode;



	
		GameNode* mTargetGameNode = nullptr;
		Dictionary<GameNode*, SharedPtr<NodeState>> mTreeNodeStatePool;



		Queue<SharedPtr<IPostProcessData>> mPostProcessQueue;


		HashSet<GameNode*> mGameNodeListToDeleteAfterCopy;
		SharedPtr<Json>    mGameNodeJsonToCopy;
	public:
		WorldHierarchyView();
		virtual ~WorldHierarchyView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	private:
		void GameNode_OnGUI(GameNode* gameNode);
		void UpdateNode();
		void PostProcess();
	private:
		void CreateEmptyObject();
		void CreateSprite();
		void CreateCube();
		void CreateSphere();
		void CreatePointLight();
		void CreateSpotLight();
		void CreateDirectionalLight();
		void CreateCamera();
		void MakePrefab();
		void Copy(bool is_remove_gamenode_after_copy = false);
		void Paste();
		bool IsCanAbleToCopy();
		bool IsCanAbleToPaste();
		bool IsCanAbleToDelete();
	private:
		NodeState* GetTreeNodeState(GameNode* gameNode);


	private:
		bool ResponseDestroyGameObject(NotifyDestroyJGObjectEvent& e);
		bool ResponseChangeGameWorld(NotifyChangeGameWorldEvent& e);
		bool ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e);
	};

}