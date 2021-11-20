#include "pch.h"
#include "GamePluginLinker.h"

#include "Class/Asset/Asset.h"
#include "Class/Global/InputManager.h"
#include "Class/Global/ProjectSetting.h"
#include "Class/Global/Scheduler.h"
#include "Class/Global/TimerManager.h"
#include "Class/Global/PhysicsManager.h"

#include "Graphics/JGGraphics.h"
#include "Graphics/Shader.h"

#include "GameObjectFactory.h"
#include "Manager/GameLayerManager.h"


namespace JG
{
	GamePluginLinker::GamePluginLinker()
	{
		Set();
	}
	void GamePluginLinker::Set()
	{
		pAssetDataBase = GetInstance<AssetDataBase>();
		pInputManager = GetInstance<InputManager>();
		pProjectSetting = GetInstance<ProjectSetting>();
		pScheduler = GetInstance<Scheduler>();
		pTimerManager = GetInstance<TimerManager>();
		pJGGraphics = GetInstance<JGGraphics>();
		pPhsicsManager = GetInstance<PhysicsManager>();
		pShaderLibrary = GetInstance<ShaderLibrary>();

		pGameObjectFactory = GetInstance<GameObjectFactory>();
		pGameLayerManager = GetInstance<GameLayerManager>();
	}

	void GamePluginLinker::Apply()
	{
		SetInstance(pAssetDataBase);
		SetInstance(pInputManager);
		SetInstance(pProjectSetting);
		SetInstance(pScheduler);
		SetInstance(pTimerManager);
		SetInstance(pJGGraphics);
		SetInstance(pPhsicsManager);
		SetInstance(pShaderLibrary);

		SetInstance(pGameObjectFactory);
		SetInstance(pGameLayerManager);
	}
}

