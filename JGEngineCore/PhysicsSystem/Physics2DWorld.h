#pragma once
#include"../EngineStatics/Engine.h"

class PyhicsBody2D;
class ENGINE_EXPORT Physics2DWorld
{
private:
	std::vector <std::shared_ptr<PyhicsBody2D>> m_vBodys;
	float m_Gravity = 0.0f;
	float m_CaculFrame = 0.0f;
public:
	Physics2DWorld();
	~Physics2DWorld();
	void InitPhysics2DWorld(const float Gravity, const float CaculFrame = 60.0f);
	void Tick(const float DeltaTime);

	float GetGravity();
};