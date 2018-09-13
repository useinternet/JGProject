#include"PhysicsWorld.h"
#include"JGShape/JGShape.h"
#include"JG2DBody.h"

#include"../EngineStatics/JGLog.h"

// юс╫ц
#include"JGCollisionCheckManager/JGCollisionCheckManager.h"
using namespace std;
PhysicsWorld::PhysicsWorld()
{
	m_CollisionManager = make_unique<JGCollisionCheckManager>(this);
}
PhysicsWorld::~PhysicsWorld()
{

}


JG2DBody* PhysicsWorld::CreateBody(JGShape* shape, const E2DBodyType type, const SJG2DBodyDesc& BodyDesc)
{
	unique_ptr<JG2DBody> body = make_unique<JG2DBody>();
	body->CreateBody(this,shape, type, BodyDesc);
	JG2DBody* result = body.get();
	m_vBodys.push_back(move(body));
	return result;
}

float PhysicsWorld::GetGravity()
{
	return m_Gravity;
}

std::vector<std::shared_ptr<JG2DBody>>* PhysicsWorld::GetBodyArray()
{
	return &m_vBodys;
}

void PhysicsWorld::Tick(const float DeltaTime, const float CheckTime)
{
	static float AccTime = 0.0f;
	AccTime += DeltaTime;
	if (AccTime >= CheckTime)
	{
		for (auto& iter : m_vBodys)
		{
			iter->GetShape()->Tick();
			if (iter->GetType() == E2DBodyType::Dyanamic)
			{
				iter->Tick(AccTime);
			}
		}
		AccTime = 0.0f;
	}
}
