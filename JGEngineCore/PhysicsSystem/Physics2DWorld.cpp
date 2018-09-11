#include"Physics2DWorld.h"
#include"PyhicsBody2D.h"

Physics2DWorld::Physics2DWorld()
{
}

Physics2DWorld::~Physics2DWorld()
{

}

void Physics2DWorld::InitPhysics2DWorld(const float Gravity, const float CaculFrame)
{
	m_Gravity = Gravity;
	m_CaculFrame = CaculFrame;
}

void Physics2DWorld::Tick(const float DeltaTime)
{
	static float AccTime = 0.0f;
	AccTime += DeltaTime;
	if (AccTime < (1.0f / m_CaculFrame))
	{
		return;
	}
	AccTime = 0.0f;
	for (auto& iter : m_vBodys)
	{
		iter->Tick(DeltaTime, m_Gravity, &m_vBodys);
	}
}
float Physics2DWorld::GetGravity()
{
	return m_Gravity;
}
