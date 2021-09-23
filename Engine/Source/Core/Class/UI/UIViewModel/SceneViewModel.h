#pragma once
#include "JGCore.h"
#include "Graphics/GraphicsDefine.h"
#include "Graphics/Resource.h"
#include "Class/UI/UIViewModel.h"


namespace JG
{
	class SceneViewModel : public UIViewModel
	{
		class SceneModel* mSceneMdoel = nullptr;
		class GameNode*   mSelectGameNode = nullptr;
	public:
		UniquePtr<Command<GameNode*>> ShowGizmo;
	public:
		virtual ~SceneViewModel() = default;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e);
	public:
		void SetMinSize(const JVector2& size);
		const JVector2& GetMinSize() const;


		void SetSceneTexture(SharedPtr<ITexture> sceneTexture);
		SharedPtr<ITexture> GetSceneTexture() const;

		GameNode* GetSelectedGameNode() const;
	public:
		void OnClick(const JVector2& pos, int mouseBt);
		//void OnClick(JVector2 pos);
	private:
		bool ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e);
		bool ResponseChangeMainSceneTexture(NotifyChangeMainSceneTextureEvent& e);
	};
}