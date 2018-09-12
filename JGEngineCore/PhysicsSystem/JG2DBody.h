#pragma once
#include"../EngineStatics/Engine.h"
class JGShape;
class PhysicsWorld;
/*
struct : SJG2DBodyDesc 
@m float Mass : 질량
@m float Friction : 마찰력
@m float Repulsive : 반발력 */
typedef struct SJG2DBodyDesc
{
	float Mass      = 0.0f;
	float Friction  = 0.0f;
	float Repulsive = 0.0f;
}JGBodyDesc;
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
Class : JG2DBody */
class ENGINE_EXPORT JG2DBody
{
private:
	JGShape* m_pShape = nullptr;
	PhysicsWorld* m_pWorld = nullptr;
	E2DBodyType m_Type;
	
	std::unique_ptr<SJG2DBodyDesc> m_BodyDesc;
	float m_Velocity = 0.0f;
	float m_Acceleration = 0.0f;
	
public:
	JG2DBody();
	~JG2DBody();
	void CreateBody(PhysicsWorld* world,JGShape* shape, const E2DBodyType type,const SJG2DBodyDesc& BodyDesc);

	void Tick(const float DeltaTime);
	JGShape* GetShape();
	E2DBodyType GetType();
	float GetVelocity();
	float GetAcceleration();
};