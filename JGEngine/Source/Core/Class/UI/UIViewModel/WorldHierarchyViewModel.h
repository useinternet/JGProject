#pragma once
#include "Class/UI/UIViewModel.h"




namespace JG
{
	class GameNode;
	struct WorldHierarchyTreeNode
	{
		GameNode* Object = nullptr;
		bool IsSelected  = false;
		bool IsTreePop = true;
		int UserFlags = 0;
	};
	class WorldHierarchyViewModel : public UIViewModel
	{
	private:
		GameWorld*  mGameWorld = nullptr;
		HashSet<GameNode*> mSelectedNodes;
		GameNode*	mCurrentSelectedNodeInContextMenu   = nullptr;
		GameNode*	mCurrentSelectedNodeInInspector     = nullptr;
	private:
		Dictionary<GameNode*, WorldHierarchyTreeNode> mTreeNodePool;
	public:
		UniquePtr<Command<GameNode*>> AddEmptyObject;
		UniquePtr<Command<GameNode*>> DeleteGameNode;
	public:
		virtual ~WorldHierarchyViewModel() = default;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	public:
		void ForEach(
			const std::function<bool(WorldHierarchyTreeNode&)>& pushAction,
			const std::function<void(WorldHierarchyTreeNode&)>& action,
			const std::function<void(WorldHierarchyTreeNode&)>& popAction);

		void SetSelectedNodeInInspector(GameNode* node);
		GameNode* GetSelectedNodeInInspector() const;

		void SetSelectedNodeInContextMenu(GameNode* node);
		GameNode* GetSelectdNodeInContextMenu() const;
	private:
		void ForEach(
			GameNode* gameNode,
			const std::function<bool(WorldHierarchyTreeNode&)>& pushAction,
			const std::function<void(WorldHierarchyTreeNode&)>& action,
			const std::function<void(WorldHierarchyTreeNode&)>& popAction);
	private:
		bool NotifyDestroyGameObject(NotifyDestroyJGObjectEvent& e);
		bool NotifyChangeGameWorld(NotifyChangeGameWorldEvent& e);
	};
}