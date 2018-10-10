#pragma once
#include"EngineFrameWork/Components/Component.h"

class PlayerStatus : public Component
{
private:
	float MaxHp;
	float CurrentHp;

	float MaxStamina;
	float CurrentStamina;

public:
	PlayerStatus();
	virtual ~PlayerStatus();
	virtual void BeginComponent(World* world);

	void SetMaxHp(const float Hp);
	void SetHp(const float Hp);
	void SetMaxStamina(const float Stamina);
	void SetStamina(const float Stamina);

	void AddMaxHp(const float Hp);
	void AddCurrentHp(const float Hp);
	void AddMaxStamina(const float Stamina);
	void AddStamina(const float Stamina);

	float GetMaxHp();
	float GetHp();
	float GetMaxStamina();
	float GetStamina();
};