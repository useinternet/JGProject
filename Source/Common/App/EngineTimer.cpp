#include "pch.h"
#include "EngineTimer.h"

using namespace std::chrono;

void EngineTimer::Start()
{
	m_IsStart = true;
}
void EngineTimer::Tick()
{
	static system_clock::time_point start_point;
	static uint32_t frame_count = 0;
	static float    time_elapsed = 0.0f;
	if (init_count < 5) init_count++;
	if (!m_IsStart)
	{
		m_DeltaTime = 0.0f;
		return;
	}




	duration<float> sec = system_clock::now() - start_point;
	m_DeltaTime = sec.count();






	if (m_DeltaTime >= 0.0f && !m_IsFirst)
	{
		m_TotalTime += m_DeltaTime;

		// fps ±¸ÇÏ±â
		++frame_count;
		time_elapsed += m_DeltaTime;
		if (time_elapsed >= 1.0f)
		{
			m_FPS = frame_count;
			frame_count = 0;
			time_elapsed = 0.0f;
		}
	}
	start_point = system_clock::now();
	m_IsFirst = false;
}
void EngineTimer::Stop()
{
	m_IsStart = false;
}

float EngineTimer::GetTotalTime(TimeStepType step_type) const
{
	switch (step_type)
	{
	case TimeStepType::MiliSecond:
		return m_TotalTime * 1000;
	case TimeStepType::Second:
		return m_TotalTime;
	case TimeStepType::Minute:
		return m_TotalTime / 60.0f;
	case TimeStepType::Hour:
		return m_TotalTime / 60.0f / 60.0f;
	}
	return 0.0f;
}
float EngineTimer::GetTick(TimeStepType step_type) const
{
	if (init_count < 5)
		return 0.0f;
	switch (step_type)
	{
	case TimeStepType::MiliSecond:
		return m_DeltaTime * 1000;
	case TimeStepType::Second:
		return m_DeltaTime;
	case TimeStepType::Minute:
		return m_DeltaTime / 60.0f;
	case TimeStepType::Hour:
		return m_DeltaTime / 60.0f / 60.0f;
	}
	return 0.0f;
}
uint32_t EngineTimer::GetFPS() const
{
	return m_FPS;
}