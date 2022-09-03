#pragma once

#include "CoreDefines.h"
#include "CoreSystem.h"
#include "Thread.h"

class GScheduleGlobalSystem : public GGlobalSystemInstance<GScheduleGlobalSystem>
{
	PList<PSharedPtr<PThread>> _thread;
public:
	GScheduleGlobalSystem();
	virtual ~GScheduleGlobalSystem() = default;

protected:
	virtual void Start() override;
	virtual void Update() override;
	virtual void Destroy() override;

	template<class ... Args>
	void ScheduleByFrame(PWeakPtr<IMemoryObject> refObject, int32 delayFrame, int32 frameCycle, int32 repeat, int32 priority, const std::function<void(const PTaskArguments&)>& func, const Args& ... args)
	{

	}

	void ScheduleByFrame(PWeakPtr<IMemoryObject> refObject, int32 delayFrame, int32 frameCycle, int32 repeat, int32 priority, const std::function<void()>& func);

	template<class ... Args>
	void Schedule(PWeakPtr<IMemoryObject> refObject, float32 delay, float32 tickCycle, int32 repeat, int32 priority, const std::function<void(const PTaskArguments&)>& func, const Args& ... args)
	{

	}

	void Schedule(PWeakPtr<IMemoryObject> refObject, float32 delay, float32 tickCycle, int32 repeat, int32 priority, const std::function<void()>& func);


	template<class ... Args>
	void ScheduleAsync(ENamedThread namedThread, PWeakPtr<IMemoryObject> refObject, const std::function<void(const PTaskArguments&)>& func, const Args& ... args)
	{

	}

	void ScheduleAsync(ENamedThread namedThread, PWeakPtr<IMemoryObject> refObject, const std::function<void()>& func);

};