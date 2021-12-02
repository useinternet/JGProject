#pragma once
#include "JGCore.h"
#include "Class/SystemLayer.h"
#include "Class/Plugin.h"
#include "Event/GameEvent.h"
#include "Event/EditorEvent.h"

namespace JG
{
	enum class EAssetFormat;
	class GlobalGameSystem;
	class GameWorld;
	class IGameObject;
	class GameLogicSystemLayer : public ISystemLayer
	{
		class ActionData : public IJGObject
		{
			JGCLASS
		public:
			std::function<void()> CompeleteAction = nullptr;
		public:
			ActionData(const std::function<void()>& compeleteAction) : CompeleteAction(compeleteAction) {}
			virtual ~ActionData() = default;
		};



		String     mGameWorldAssetPath;
		GameWorld* mGameWorld = nullptr;
		bool       mIsGamePlaying = false;
		List<GlobalGameSystem*> mGameSystemList;


		UniquePtr<Plugin> mGamePlugin;




		// 21 * 9 * 32
		SharedPtr<IComputer> mComputer;
		List<JBBox> mClusters;
		i32 numXSlice = 8;
		i32 numYSlice = 1;
		i32 numZSlice = 32;
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
		bool ResponsePlayGame(RequestPlayGameEvent& e);
		bool ResponsePauseGame(RequestPauseGameEvent& e);
		bool ResponseStopGame(RequestStopGameEvent& e);
		bool ResponseEditorSceneOnClick(NotifyEditorSceneOnClickEvent& e);
	private:
		void RegisterGlobalGameSystem();
		void RegisterGameObjectType();
		void Test();
	};
}