#pragma once
#include"EngineFrameWork/AI/BT_Task.h"


class BTT_MoveTo : public BT_Task
{
private:

public:
	BTT_MoveTo();
	virtual ~BTT_MoveTo();
	virtual bool ExcuteTask(const float DeltaTime) override;
};