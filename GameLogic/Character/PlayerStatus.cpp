#include"PlayerStatus.h"

PlayerStatus::PlayerStatus()
{
	RegisterComponentID(typeid(this));

	MaxHp = 1000.0f;
	MaxStamina = 1000.0f;
}

PlayerStatus::~PlayerStatus()
{
}

void PlayerStatus::BeginComponent(World* world)
{
	Component::BeginComponent(world);
	CurrentHp = MaxHp;
	CurrentStamina = MaxStamina;
}

void PlayerStatus::SetMaxHp(const float Hp)
{
	MaxHp = Hp;
}

void PlayerStatus::SetHp(const float Hp)
{
	CurrentHp = Hp;
}

void PlayerStatus::SetMaxStamina(const float Stamina)
{
	MaxStamina = Stamina;
}

void PlayerStatus::SetStamina(const float Stamina)
{
	CurrentStamina = Stamina;
}

void PlayerStatus::AddMaxHp(const float Hp)
{
	MaxHp += Hp;
}

void PlayerStatus::AddCurrentHp(const float Hp)
{
	CurrentHp += Hp;
}

void PlayerStatus::AddMaxStamina(const float Stamina)
{
	MaxStamina += Stamina;
}

void PlayerStatus::AddStamina(const float Stamina)
{
	CurrentStamina = Stamina;
}

float PlayerStatus::GetMaxHp()
{
	return MaxHp;
}

float PlayerStatus::GetHp()
{
	return CurrentHp;
}

float PlayerStatus::GetMaxStamina()
{
	return MaxStamina;
}

float PlayerStatus::GetStamina()
{
	return CurrentStamina;
}
