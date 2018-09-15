#pragma once
#include"../EngineStatics/Engine.h"

class PhysicsWorld;
class ENGINE_EXPORT PhysicsCalculationSystem
{
private:
	PhysicsWorld* m_pPhysicsWorld = nullptr;


public:
	PhysicsCalculationSystem() = delete;
	PhysicsCalculationSystem(PhysicsWorld* pWorld);
	~PhysicsCalculationSystem();


	void Calculation(const float DeltaTime);
};