#pragma once

#include <chrono>



enum class TimeStepType
{
	Hour,
	Minute,
	Second,
	MiliSecond
};
class EngineTimer
{
public:
	void Start();
	void Tick();
	void Stop();

	float GetTotalTime(TimeStepType step_type = TimeStepType::Second) const;
	float GetTick(TimeStepType step_type = TimeStepType::Second) const;
	uint32_t GetFPS() const;
private:
	float m_TotalTime = 0.0f;
	float m_DeltaTime = 0.0f;
	uint32_t m_FPS = 0;
	bool m_IsStart = false;
	bool m_IsFirst = true;
	uint32_t init_count = 0;
};