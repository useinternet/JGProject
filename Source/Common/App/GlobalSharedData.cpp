#include "pch.h"
#include "GlobalSharedData.h"

using namespace std;
//#pragma data_seg("SHAREDATA")

shared_ptr<GlobalSharedData> g_sharedData = nullptr;

//#pragma data_seg()
//#pragma comment( linker, "/SECTION:SHAREDATA,RWS" )




MessageBus* GlobalSharedData::GetMessageBus()
{
	return g_sharedData->msgBus;
}

EventManager* GlobalSharedData::GetEventManager()
{
	return g_sharedData->eventManager;
}

JWindowManager* GlobalSharedData::GetWindowManager()
{
	return g_sharedData->winManager;
}

LogSystem* GlobalSharedData::GetLogSystem()
{
	return g_sharedData->logSystem;
}

EngineTimer* GlobalSharedData::GetEngineTimer()
{
	return g_sharedData->engineTimer;
}

AssetManager* GlobalSharedData::GetAssetManager()
{
	return g_sharedData->assetManager;
}

void LinkSharedData(const GlobalSharedData& data)
{
	g_sharedData = make_shared<GlobalSharedData>();
	*g_sharedData = data;
}
const GlobalSharedData& GetGlobalSharedData()
{
	return *g_sharedData;
}

