#pragma once
#include "JGCore.h"
#include "Class/SystemLayer.h"

namespace JG
{
	enum class EAssetFormat;
	class GlobalGameSystem;
	class GameWorld;
	class IGameObject;
	class GameLogicSystemLayer : public ISystemLayer
	{
		class LoadGameWorldData : public IJGObject
		{
			JGCLASS
		public:
			String     Path;
			GameWorld* GameWorld = nullptr;
			SharedPtr<Json> Json = nullptr;
			bool IsSuccessed = false;
		public:
			LoadGameWorldData(const String& path) : Path(path) {}
		};
		class SaveGameWorldData : public IJGObject
		{
			JGCLASS
		public:
			String     Path;
			GameWorld* GameWorld = nullptr;
			SharedPtr<Json> Json = nullptr;
			bool IsSuccessed = false;
		};



		SharedPtr<IAsset> mGameWorldAssetInstance = nullptr;
		GameWorld* mGameWorld = nullptr;
		List<GlobalGameSystem*> mGameSystemList;


		UniquePtr<TaskController> mLoadGameWorldTaskCtrl;
		UniquePtr<TaskController> mSaveGameWorldTaskCtrl;
	public:
		virtual ~GameLogicSystemLayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void Begin() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e) override;
		virtual String GetLayerName() override;
	public:
		bool ResponseSaveGameWorld(RequestSaveGameWorldEvent& e);
		bool ResponseLoadGameWorld(RequestLoadGameWorldEvent& e);
		bool ResponseNotfyRenderingReadyCompelete(NotifyRenderingReadyCompeleteEvent& e);
		bool ResponseEditorSceneOnClick(NotifyEditorSceneOnClickEvent& e);
	public:

	private:
		void RegisterGlobalGameSystem();
		void RegisterGameObjectType();
	private:
		//void SaveGameWorld();
		//void LoadGameWrold();
	};
}