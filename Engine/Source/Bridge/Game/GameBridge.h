#pragma once


#ifndef GAME_API
#ifdef GAME_DLL_EXPORT
#define GAME_API extern "C" __declspec(dllexport)
#else
#define GAME_API extern "C" __declspec(dllimport)
#endif
#endif 

#include "JGCore.h"
#include "GameObjectFactory.h"
#include "Class/Plugin.h"
GAME_API void Start_Plugin(JG::SharedPtr<JG::PluginLinker> linker);

// 게임 컴포넌트 
//x##
#define REGISTER_GAMECOMPONENT(x) \
void x_Register_GameComponent() \
{ \
	JG::GameObjectFactory::GetInstance().RegisterComponentType<##x>(); \
} \

