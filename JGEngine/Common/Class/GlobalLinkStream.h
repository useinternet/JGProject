#pragma once

#include "CommonCore.h"
#include "EngineTimer.h"
#include <memory>
struct GlobalLinkStream
{
	std::function<void(Event& e)>   OnEvent;
	std::shared_ptr<spdlog::logger> Logger;
	std::string LogFileName;
	std::shared_ptr<EventManager>   EngineEventManager;
	std::shared_ptr<EngineTimer>    _EngineTimer;
	std::shared_ptr<EngineConfig>   _EngineConfig;
	std::shared_ptr<EnginePerformance> _EnginePerformance;
};

class GlobalLinkData
{
	static std::shared_ptr<EngineTimer>    _EngineTimer;
public:
	static std::function<void(Event& e)>   OnEvent;
	static std::shared_ptr<spdlog::logger> Logger;
	static std::shared_ptr<EventManager>   EngineEventManager;
	static std::shared_ptr<EngineConfig>   _EngineConfig;
	static std::shared_ptr<EnginePerformance> _EnginePerformance;
	static void Init(const GlobalLinkStream& stream, bool is_app = false)
	{
		OnEvent = stream.OnEvent;
		Logger = stream.Logger;
		EngineEventManager = stream.EngineEventManager;
		_EngineTimer = stream._EngineTimer;
		_EngineConfig = stream._EngineConfig;
		_EnginePerformance = stream._EnginePerformance;
		if(!is_app)
			Log::RegisterLog(Logger, stream.LogFileName);
	}
	static GlobalLinkStream GetStream()
	{
		GlobalLinkStream stream;
		stream.OnEvent = OnEvent;
		stream.Logger = Logger;
		stream.EngineEventManager = EngineEventManager;
		stream._EngineTimer = _EngineTimer;
		stream._EngineConfig = _EngineConfig;
		stream._EnginePerformance = _EnginePerformance;
		return stream;
	}

	static float GetTick(TimeStepType type = TimeStepType::Second);
	static float GetTotalTime(TimeStepType type = TimeStepType::Second);
	static uint32_t GetFPS();
};