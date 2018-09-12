#pragma once
#include"../EngineStatics/Engine.h"


// 물리 계획
/*

// 가속도, 속도 
가속도는 속도를 변화 시키고
속도는 위치를 변화시킨다.


F = ma
물체는 
( 도형을 가진다. )
무게
가속도 : 질량 / 힘

*/
// 중력 -9.8f;
// 마찰력 = 수직항력 * 마찰 계수
// 가속도
// 반발력 = 탄성계수 * 떨어진 높이

// 
// 피봇에따라 b2Vec2 -> JGVector2D;
// 애니메이션, 스태틱



class ENGINE_EXPORT PhysicsSystem
{
private:
	float m_GravityConstant = 0.0f;
public:
	PhysicsSystem();
	~PhysicsSystem();

	void InitPhysicsSystem(const float gravity);
};