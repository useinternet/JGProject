#pragma once
#include "JGCore.h"

namespace JG
{
	class GamePluginLinker : public IGlobalSingletonLinker
	{
		class AssetDataBase*	pAssetDataBase = nullptr;
		class InputManager*		pInputManager = nullptr;
		class ProjectSetting*	pProjectSetting = nullptr;
		class Scheduler*		pScheduler = nullptr;
		class TimerManager*		pTimerManager = nullptr;
		class JGGraphics*		pJGGraphics = nullptr;
		class PhysicsManager*	pPhsicsManager = nullptr;
		class ShaderLibrary*	pShaderLibrary = nullptr;

		class GameObjectFactory* pGameObjectFactory = nullptr;
		class GameLayerManager* pGameLayerManager   = nullptr;
	public:
		GamePluginLinker();
		virtual void Set()   override;
		virtual void Apply() override;
	};


}