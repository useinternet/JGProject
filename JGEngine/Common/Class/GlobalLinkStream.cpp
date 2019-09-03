#include "pch.h"
#include "GlobalLinkStream.h"

std::function<void(Event& e)>   GlobalLinkData::OnEvent;
std::shared_ptr<spdlog::logger> GlobalLinkData::Logger;
std::shared_ptr<EventManager>   GlobalLinkData::EngineEventManager;
std::shared_ptr<EngineTimer>    GlobalLinkData::_EngineTimer;
std::shared_ptr<EngineConfig>   GlobalLinkData::_EngineConfig;
std::shared_ptr<EnginePerformance> GlobalLinkData::_EnginePerformance;

float GlobalLinkData::GetTick(TimeStepType type) {
	return _EngineTimer->GetTick(type);
}
float GlobalLinkData::GetTotalTime(TimeStepType type) {
	return _EngineTimer->GetTotalTime(type);
}
uint32_t GlobalLinkData::GetFPS() {
	return _EngineTimer->GetFPS();
}