#pragma once


#include "Class/UI/UIView.h"
#include "Class/UI/UIViewModel/WorldHierarchyViewModel.h"

namespace JG
{

	class GameWorld;
	class WorldHierarchyView : public UIView<WorldHierarchyViewModel>
	{
		JGCLASS


		enum
		{
			NodeState_Selected = 0x0001,
			NodeState_Renaming = 0x0002,
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

		bool mOpenGUI = true;
		bool mIsCtrl = false;
		GameWorld* mGameWorld = nullptr;

		HashSet<GameNode*> mSelectedGameNode;
		GameNode* mTargetGameNode = nullptr;
		Dictionary<GameNode*, SharedPtr<NodeState>> mTreeNodeStatePool;
		
		//WorldHierarchyViewModel* mVm = nullptr;
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
	private:
		void CreateEmptyObject();
		void CreateSprite();
		void CreateCube();
		void CreateSphere();
		void CreatePointLight();
	private:
		NodeState* GetTreeNodeState(GameNode* gameNode);
	private:
		bool ResponseDestroyGameObject(NotifyDestroyJGObjectEvent& e);
		bool ResponseChangeGameWorld(NotifyChangeGameWorldEvent& e);
		bool ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e);
	//private:
	//	void UpdateDragAndDrop(GameNode* targetNode);
	};

}