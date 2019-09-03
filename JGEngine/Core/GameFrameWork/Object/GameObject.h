#pragma once

#include "GFW.h"




class GAMEFRAMEWORK_API GameObject
{


public:
	virtual void Begin();
	virtual void Tick();
	virtual void End();

protected:
	std::shared_ptr<spdlog::logger> GetLogger();
};

