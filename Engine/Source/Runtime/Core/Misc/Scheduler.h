#pragma once

#include "CoreDefines.h"
#include "CoreSystem.h"


// c++ 20 �ڷ�ƾ�� ÷��

// Schedule Handle
// �Ҵ�
class GScheduleGlobalSystem : public GGlobalSystemInstance<GScheduleGlobalSystem>
{

public:
	// Schedule -> Time
	// ScheduleByFrame -> Frame
	// ScheduleAsync -> thread, flag�� ���� ������ �Ѿ������ ��ٸ��Ⱑ �ִ�.
};