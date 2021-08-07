#include "pch.h"
#include "SceneViewModel.h"
#include "Class/UI/UIModel/SceneModel.h"


namespace JG
{
	void SceneViewModel::Initialize()
	{
		mSceneMdoel = RegisterUIModel<SceneModel>();
		ShowGizmo = CreateUniquePtr<Command<GameNode*>>();
		
	}

	void SceneViewModel::Destroy()
	{
		UIViewModel::Destroy();
		ShowGizmo->UnSubscribe(this);
	}

	void SceneViewModel::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<NotifyChangeMainSceneTextureEvent>(EVENT_BIND_FN(&SceneViewModel::ResponseChangeMainSceneTexture));
		dispatcher.Dispatch<NotifySelectedGameNodeInEditorEvent>(EVENT_BIND_FN(&SceneViewModel::ResponseSelectedGameNodeInEditor));
	}

	void SceneViewModel::SetMinSize(const JVector2& size)
	{
		mSceneMdoel->SetMinSize(size);
	}

	const JVector2& SceneViewModel::GetMinSize() const
	{
		return mSceneMdoel->GetMinSize();
	}

	void SceneViewModel::SetSceneTexture(SharedPtr<ITexture> sceneTexture)
	{
		mSceneMdoel->SetSceneTexture(sceneTexture);
	}
	SharedPtr<ITexture> SceneViewModel::GetSceneTexture() const
	{
		return mSceneMdoel->GetSceneTexture();
	}
	GameNode* SceneViewModel::GetSelectedGameNode() const
	{
		return mSelectGameNode;
	}
	void SceneViewModel::OnClick(const JVector2& pos, int mouseBt)
	{
		NotifyEditorSceneOnClickEvent e;
		e.ClickPos = pos;
		SendEvent(e);
		// 클릭을 하게 되면 -> 인스턴스 얻어오기
	}
	bool SceneViewModel::ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e)
	{
		if (e.SelectedGameNode == nullptr)
		{
			return false;
		}
		mSelectGameNode = e.SelectedGameNode;
		return false;
	}
	bool SceneViewModel::ResponseChangeMainSceneTexture(NotifyChangeMainSceneTextureEvent& e)
	{
		if (mSceneMdoel != nullptr)
		{
			mSceneMdoel->SetSceneTexture(e.SceneTexture);
		}
		return false;
	}
}

