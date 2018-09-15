#pragma once
#include"../EngineStatics/Engine.h"

enum class E2DBodyType;
struct SJG2DBodyDesc;
class JG2DBody;
class JGShape;

//임시
class JGCollisionCheckManager;
class PhysicsCalculationSystem;
class ENGINE_EXPORT PhysicsWorld
{

private:
	std::vector<std::shared_ptr<JG2DBody>> m_vBodys;

	// 실험 목적용 충돌 매니저..(나중에 따로 분리해서.. 파직스 시스템에 연결..)
	std::unique_ptr<JGCollisionCheckManager> m_CollisionManager;
	std::unique_ptr<PhysicsCalculationSystem> m_CaluculSystem;
	float m_Gravity = 0.1f;
public:
	PhysicsWorld();
	~PhysicsWorld();


	JG2DBody* CreateBody(JGShape* shape, const E2DBodyType type);
	float GetGravity();
	std::vector<std::shared_ptr<JG2DBody>>* GetBodyArray();
	void Tick(const float DeltaTime, const float CheckTime);
};