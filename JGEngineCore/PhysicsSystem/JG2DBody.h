#pragma once
#include"../EngineStatics/Engine.h"
class JGVector2D;
class JGShape;
class PhysicsWorld;
class JGCollisionCheck;
class JGCollisionCheckManager;
class PhysicsCalculationSystem;

/*
EnumClass : E2DBodyType
@e Static : 정적 몸체
@e Dyanamic : 동적 몸체 */
enum class E2DBodyType
{
	Static,
	Dyanamic
};
/*
Class : JG2DBody 
@m unique_ptr<JGCollisionCheck> m_CollisionChecker : 충돌 체커
@m JGShape* m_pShape : 몸체의 도형 
@m PhysicsWorld* m_pWorld : 속해있는 물리 세계
@m 
@m
@m
   */
class ENGINE_EXPORT JG2DBody
{
private:
	friend PhysicsWorld;
	friend JGCollisionCheckManager;
	friend PhysicsCalculationSystem;
private:
	PhysicsWorld * m_pWorld = nullptr;
	std::unique_ptr<JGCollisionCheck> m_CollisionChecker;
	JGShape* m_pShape = nullptr;

	E2DBodyType m_Type;
	std::unique_ptr<JGVector2D> m_Force;
	std::unique_ptr<JGVector2D> m_AccForce;
	
public:
	JG2DBody();
	~JG2DBody();
	/*
	Exp : JGCollisionCheck를 만든다. */
	JGCollisionCheck* CreateCollisionChecker();
	/*
	Exp : JGCollisionCheck를 가져온다. */
	JGCollisionCheck* GetCollisionChecker();
	/*
	Exp : 몸체의 도형 정보를 가져온다. */
	JGShape* GetShape();
	/*
	Exp : 몸체의 타입을 가져온다. */
	E2DBodyType GetType();
	/*
	Exp : 현재 속력을 가져온다. */
	float GetVelocity();
	/*
	Exp : 현재 가속도를 가져온다. */
	float GetAcceleration();
	/*
	Exp : 현재 가해진 힘을 가져온다. */
	JGVector2D& GetForce();

protected:
	void CreateBody(PhysicsWorld* world, JGShape* shape, const E2DBodyType type);
	void AddForce(const float x, const float y);
	void AddForce(const JGVector2D& vec);
	void AddForceX(const float x);
	void AddForceY(const float y);
	void SetForce(const float x, const float y);
	void SetForce(const JGVector2D& vec);
	void Tick(const float DeltaTime);
};