#pragma once

#include "CoreDefines.h"
#include "CoreSystem.h"


// c++ 20 코루틴도 첨가

// Schedule Handle
// 할당
class GScheduleGlobalSystem : public GGlobalSystemInstance<GScheduleGlobalSystem>
{

public:
	// Schedule -> Time
	// ScheduleByFrame -> Frame
	// ScheduleAsync -> thread, flag로 다음 프레임 넘어갈때까지 기다리기가 있다.
};