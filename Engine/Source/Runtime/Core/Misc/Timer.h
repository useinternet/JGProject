#pragma once

#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "SequentialIDGenerator.h"

class GTimerGlobalSystem;

enum class ETimeStepType
{
	Hour,
	Minute,
	Second,
	MiliSecond
};

class PTimer : public IMemoryObject
{
	friend GTimerGlobalSystem;
private:
	uint64 ID = INDEX_NONE;
	HSteadyClock TimePoint;
	float32 TotalTime = 0.0f;
	float32 DeltaTime = 0.0f;
	float32 TimeElapsed = 0.0f;
	uint32 FPS = 0;
	uint32 FrameCount = 0;
	bool bIsRun   = false;
	bool bIsFirst = true;

private:
	void Tick();

public:
	void Start();
	void Stop();
	float32 GetTotalTime(ETimeStepType stepType = ETimeStepType::Second) const;
	float32 GetTick(ETimeStepType stepType = ETimeStepType::Second) const;
	uint64  GetFPS() const;
	bool    IsValid() const;

public:
	static PSharedPtr<PTimer> Create();





public:
	virtual ~PTimer() = default;
};


class GTimerGlobalSystem : public GGlobalSystemInstance<GTimerGlobalSystem>
{
	HMutex _mutex;
	HList<PWeakPtr<PTimer>> _timers;
	PSequentialIDGenerator  _idGenerator;
public:
	GTimerGlobalSystem() = default;
	virtual ~GTimerGlobalSystem() = default;

public:
	PSharedPtr<PTimer> CreateTimer();
};



