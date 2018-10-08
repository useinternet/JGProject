#include "EnemyUnitBase.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"
EnemyUnitBase::EnemyUnitBase()
{
	RegisterObjectID(typeid(this));
	GetCollision()->SetAsBox(50.0f, 200.0f);
	GetCollision()->SetComponentLocation(1400.0f, 800.0f);
}

EnemyUnitBase::~EnemyUnitBase()
{

}
