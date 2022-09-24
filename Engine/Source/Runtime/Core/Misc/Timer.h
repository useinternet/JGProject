#pragma once

#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "SequentialIDGenerator.h"

class GTimerGlobalSystem;
class PTimer : public IMemoryObject
{
	friend GTimerGlobalSystem;

private:
	uint64 _id = INDEX_NONE;
	HSteadyClock_TimePoint _timePoint;
	float32 _totalTime = 0.0f;
	float32 _deltaTime = 0.0f;
	float32 _timeElapsed = 0.0f;
	uint32 _fps = 0;
	uint32 _frameCount = 0;
	bool _bIsRun   = false;
	bool _bIsFirst = true;

public:
	virtual ~PTimer() = default;

private:
	void Update();

public:
	void Start();
	void Stop();
	void Reset();
	float32 GetTotalTime() const;
	float32 GetTick() const;
	uint64  GetFPS() const;
	bool    IsValid() const;

public:
	static PSharedPtr<PTimer> Create();
};


class GTimerGlobalSystem : public GGlobalSystemInstance<GTimerGlobalSystem>
{
	HMutex _mutex;
	HList<PWeakPtr<PTimer>> _timers;
	PSequentialIDGenerator  _idGenerator;
public:
	virtual ~GTimerGlobalSystem() = default;

protected:
	virtual void Update() override;

public:
	PSharedPtr<PTimer> CreateTimer();
};



