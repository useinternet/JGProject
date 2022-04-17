#pragma once


#ifndef GAME_API
#ifdef GAME_DLL_EXPORT
#define GAME_API __declspec(dllexport)
#define GAME_C_API extern "C" __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#define GAME_C_API extern "C" __declspec(dllimport)
#endif
#endif 

#include "JGCore.h"
#include "GameObjectFactory.h"
#include "GameComponent.h"
#include "Class/Plugin.h"
GAME_C_API void Start_Plugin(JG::SharedPtr<JG::PluginLinker> linker);

// 게임 컴포넌트 
//x##
#define REGISTER_GAMECOMPONENT(x) \
class x; \
GAME_C_API void x##_Register_GameComponent() \
{ \
	JG::GameObjectFactory::GetInstance().RegisterComponentType<##x>(); \
} \

#define JG_GAMECOMPONENT \
	JGCLASS \

