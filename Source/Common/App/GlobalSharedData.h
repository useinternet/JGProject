#pragma once





/* 
* 2020.06.07
* Class Name : GlobalSharedData
* dll 간 전역 공유 데이터 
*/









#include "CommonCore.h"
class MessageBus;
class EventManager;
class JWindowManager;
class Application;
class LogSystem;
class EngineTimer;

class GlobalSharedData
{
public:
	static MessageBus*     GetMessageBus();
	static EventManager*   GetEventManager();
	static JWindowManager* GetWindowManager();
	static LogSystem*      GetLogSystem();
	static EngineTimer*    GetEngineTimer();
private:
	friend Application;
private:
	MessageBus*     msgBus       = nullptr;
	EventManager*   eventManager = nullptr;
	JWindowManager* winManager   = nullptr;
	LogSystem*      logSystem    = nullptr;
	EngineTimer*    engineTimer  = nullptr;
};

void  LinkSharedData(const GlobalSharedData& data);
const GlobalSharedData& GetGlobalSharedData();

using GlobalSharedDataLinkFunc = void* (*)(const GlobalSharedData&);


#define LINK_GLOBAL_SHARED_DATA(plugin)  ((GlobalSharedDataLinkFunc)plugin->GetProcAddress("LinkGlobalSharedData"))(GetGlobalSharedData());
#define REGISTER_GLOBAL_SHARED_DATA \
extern "C" __declspec(dllexport) void LinkGlobalSharedData(const GlobalSharedData & data)\
{\
LinkSharedData(data); \
}\
\



