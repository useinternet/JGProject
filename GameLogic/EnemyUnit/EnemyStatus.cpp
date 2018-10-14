#include"EnemyStatus.h"


EnemyStatus::EnemyStatus()
{
	RegisterComponentID(typeid(this));


}
EnemyStatus::~EnemyStatus()
{

}

void EnemyStatus::BeginComponent(World * world)
{
	Component::BeginComponent(world);

	CurrentHp = MaxHp;
}

void EnemyStatus::SetMaxHp(const float Hp)
{
	MaxHp = Hp;
}

void EnemyStatus::SetCurrentHp(const float Hp)
{
	CurrentHp = Hp;
}

void EnemyStatus::AddMaxHp(const float Hp)
{
	MaxHp += Hp;
}

void EnemyStatus::AddCurrentHp(const float Hp)
{
	CurrentHp += Hp;
}

float EnemyStatus::GetMaxHp()
{
	return MaxHp;
}

float EnemyStatus::GetCurrentHp()
{
	return CurrentHp;
}
