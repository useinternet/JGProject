#pragma once
#include "JGCore.h"

namespace JG
{
	enum class EPhysicsAPI
	{
		PhysX
	};
	class IPhysicsAPI
	{
	protected:
		friend class Application;
		virtual bool Create() = 0;
		virtual void Destroy() = 0;
	public:
		static UniquePtr<IPhysicsAPI> Create(EPhysicsAPI api);
	};
}