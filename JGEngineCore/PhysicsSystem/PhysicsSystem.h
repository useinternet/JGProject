#pragma once
#include"../EngineStatics/Engine.h"


// 물리 계획
/*
b2world (world당 하나)        


// 물리 적용 방법 
1. b2World 생성( 중력값 )
2. b2BodyDef 생성( 바디 값 설정)
3. b2Body* 선언후 = b2World::CreateBody로 생성
4. b2PolygonShape나 각종 도형 클래스로 CreateFixture로 생성;

// b2FixtureDef 로 다이나믹 오브젝트 생성 가능
// b2Fixture클래스는 이것도 CreateFixture로 생성.. 

*/




class ENGINE_EXPORT PhysicsSystem
{
private:
	std::map<const std::wstring, std::unique_ptr<b2World>> m_mPhysicsWorlds;
	std::unique_ptr<b2World> m_PhysicsWorld;


public:
	PhysicsSystem();
	~PhysicsSystem();

	void InitPhysicsSystem(const float gravity);
};