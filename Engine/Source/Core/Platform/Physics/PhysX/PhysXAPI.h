#pragma once
#include "Physcis/PhysicsAPI.h"

namespace JG
{
	class PhysXAPI : public IPhysicsAPI
	{
	private:
		friend class Application;
		virtual bool Create() override;
		virtual void Destroy() override;
	};
}