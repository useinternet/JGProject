#pragma once


#include "Class/UI/UIView.h"
#include "Class/UI/UIViewModel/WorldHierarchyViewModel.h"

namespace JG
{

	class GameWorld;
	class WorldHierarchyView : public UIView<WorldHierarchyViewModel>
	{
		JGCLASS

		bool mOpenGUI = true;
		GameWorld* mGameWorld = nullptr;




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
	private:
		bool ResponseDestroyGameObject(NotifyDestroyJGObjectEvent& e);
		bool ResponseChangeGameWorld(NotifyChangeGameWorldEvent& e);
		bool ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e);
	//private:
	//	void UpdateDragAndDrop(GameNode* targetNode);
	};

}