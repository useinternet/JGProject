#pragma once

#ifdef JGGAME_EXPORTS

#define GAME_API __declspec(dllexport)
#include "CommonCore.h"
#include "Object/GameObject.h"


#define JGCLASS(class_name)  \
extern "C" GAME_API GameObject* Create##class_name() { \
     GameObject* obj = new(##class_name); \
     return obj; \
} \



#define JG_LOG(loglevel, str, ...) GetLogger()->##loglevel("[Game] : " + std::string(str), __VA_ARGS__)\


#define JG_LOG_TRACE(str, ...) JG_LOG(trace, str, __VA_ARGS__)
#define JG_LOG_WARN(str, ...) JG_LOG(warn, str, __VA_ARGS__)
#define JG_LOG_INFO(str, ...) JG_LOG(info, str, __VA_ARGS__)
#define JG_LOG_ERROR(str, ...) JG_LOG(error, str, __VA_ARGS__)
#define JG_LOG_FATAL(str, ...) JG_LOG(critical, str, __VA_ARGS__)


#endif









