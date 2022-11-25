#include "PCH/PCH.h"
#include "Timer.h"
#include "Thread/Scheduler.h"

void PTimer::Update()
{
	if (_bIsFirst)
	{
		_timePoint = HSteadyClock::now();
		_bIsFirst = false;
		return;
	}

	if (_bIsRun == false)
	{
		_deltaTime  = 0.0f;
		_frameCount = 0;

		_timePoint = HSteadyClock::now();
		return;
	}
	
	HDuration<float32> sec = HSteadyClock::now() - _timePoint;
	_deltaTime = sec.count();

	if (_deltaTime >= 0.0f)
	{
		_totalTime   += _deltaTime;
		_timeElapsed += _deltaTime;
		_frameCount  += 1;

		if (_timeElapsed >= 1.0f)
		{
			_fps = _frameCount;
			_frameCount  = 0;
			_timeElapsed = 0.0f;
		}
	}

	_timePoint = HSteadyClock::now();
}

void PTimer::Start()
{
	_bIsRun = true;
}

void PTimer::Stop()
{
	_bIsRun = false;
}

void PTimer::Reset()
{
	_totalTime = 0.0f;
	_deltaTime = 0.0f;
	_timeElapsed = 0.0f;
	_fps = 0;
	_frameCount = 0;
	_bIsRun   = true;
	_bIsFirst = false;
}

float32 PTimer::GetTotalTime() const
{
	return _totalTime;
}

float32 PTimer::GetTick() const
{
	return _deltaTime;
}

uint64 PTimer::GetFPS() const
{
	return _fps;
}

bool PTimer::IsValid() const
{
	return _id != INDEX_NONE;
}

PSharedPtr<PTimer> PTimer::Create()
{
	return GTimerGlobalSystem::GetInstance().CreateTimer();
}

void GTimerGlobalSystem::Update()
{
	int32 cnt = (int32)_timers.size();

	for (int32 i = 0; i < cnt;)
	{
		PSharedPtr<PTimer> timer = _timers[i].Pin();
		if (timer.IsValid() == true)
		{
			timer->Update();
			
			++i;
		}
		else
		{
			PWeakPtr<PTimer> temp = _timers[i];
			_timers[i] = _timers[cnt - 1];
			_timers[cnt - 1] = temp;

			_timers.pop_back();
			cnt = (int32)_timers.size();
		}
	}
}

PSharedPtr<PTimer> GTimerGlobalSystem::CreateTimer()
{
	PSharedPtr<PTimer> result = Allocate<PTimer>();
	result->_id = _idGenerator.NewID();

	_timers.push_back(result);
	return result;
}
