#pragma once
#ifdef PHYSICSENGINE_EXPORT

#define PHYSICSENGINE_API __declspec(dllexport)

#else

#define PHYSICSENGINE_API __declspec(dllimport)

#endif
#include "CommonCore.h"
namespace PE
{
	REGISTER_SYSTEM_CORE(PhysicsEngine, 0, 0, 0)

	class PHYSICSENGINE_API PhysicsEngine : public SystemCore
	{
	public:
		PhysicsEngine(const SystemCoreDesc& desc) : SystemCore(desc) {}
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void Destroy() override;
	public:
		virtual void ReceiveMessage(const Message& msg) override;


	private:



	};
}