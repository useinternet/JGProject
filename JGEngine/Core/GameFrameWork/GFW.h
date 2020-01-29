#pragma once

#pragma warning(disable : 4251 4275 26495 26451 6387 26439 26498)

#ifdef GAMEFRAMEWORK_EXPORTS
#define GAMEFRAMEWORK_API __declspec(dllexport)
#else
#define GAMEFRAMEWORK_API __declspec(dllimport)
#endif
#include "CommonCore.h"



class GameObject;
typedef  GameObject* (*pCreateGameObjectFunc)();
typedef  void (*pDeleteGameObjectFunc)(GameObject*);