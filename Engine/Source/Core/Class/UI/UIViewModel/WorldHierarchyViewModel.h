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
		HashSet<GameNode*> mCurrentSelectedNodeList;


		SharedPtr<Json> mCopyJson = nullptr;
	private:
		Dictionary<GameNode*, WorldHierarchyTreeNode> mTreeNodePool;
	public:
		UniquePtr<Command<>> CreateEmptyObject;
		UniquePtr<Command<>> CreateSprite;
		UniquePtr<Command<>> CreateCube;
		UniquePtr<Command<>> CreateSphere;
		UniquePtr<Command<>> CreatePointLight;
		UniquePtr<Command<>> CopyGameNodes;
		UniquePtr<Command<>> PasteGameNodes;
		UniquePtr<Command<>> DeleteGameNode;
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


		void AddSelectedNode(GameNode* node);
		void ClearSelectedNode();
		const HashSet<GameNode*>& GetSelectedNodeList() const;
	private:
		void ForEach(
			GameNode* gameNode,
			const std::function<bool(WorldHierarchyTreeNode&)>& pushAction,
			const std::function<void(WorldHierarchyTreeNode&)>& action,
			const std::function<void(WorldHierarchyTreeNode&)>& popAction);
	private:
		bool ResponseDestroyGameObject(NotifyDestroyJGObjectEvent& e);
		bool ResponseChangeGameWorld(NotifyChangeGameWorldEvent& e);
		bool ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e);
	};
}