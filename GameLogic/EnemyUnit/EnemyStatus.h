#pragma once
#include"EngineFrameWork/Components/Component.h"

class EnemyStatus : public Component
{
private:
	float MaxHp = 0.0f;
	float CurrentHp = 0.0f;
public:
	EnemyStatus();
	virtual ~EnemyStatus();
	virtual void BeginComponent(World* world) override;


	void SetMaxHp(const float Hp);
	void SetCurrentHp(const float Hp);

	void AddMaxHp(const float Hp);
	void AddCurrentHp(const float Hp);

	float GetMaxHp();
	float GetCurrentHp();
};