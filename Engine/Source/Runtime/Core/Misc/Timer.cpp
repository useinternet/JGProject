#include "Timer.h"


void PTimer::Tick()
{

}

void PTimer::Start()
{

}

void PTimer::Stop()
{

}

float32 PTimer::GetTotalTime(ETimeStepType stepType) const
{
	return 0.0f;
}

float32 PTimer::GetTick(ETimeStepType stepType) const
{
	return 0.0f;
}

uint64 PTimer::GetFPS() const
{
	return 0;
}

bool PTimer::IsValid() const
{
	return false;
}

PSharedPtr<PTimer> PTimer::Create()
{
	return nullptr;
}