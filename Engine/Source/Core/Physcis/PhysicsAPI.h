#pragma once
#include "JGCore.h"

namespace JG
{

	class IPhysicsAPI
	{
	protected:
		friend class Application;
		virtual bool Create() = 0;
		virtual void Destroy() = 0;
	};
}