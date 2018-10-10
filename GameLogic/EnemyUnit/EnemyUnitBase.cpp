#include "EnemyUnitBase.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"StaticFilter/StaticCollisionFilter.h"
EnemyUnitBase::EnemyUnitBase()
{
	RegisterObjectID(typeid(this));
	GetCollision()->SetCategoryFilter(Filter_Enemy);
	GetCollision()->SetMaskFilter(Filter_Ground | Filter_PlayerAttack | Filter_PlayerCollision);
	GetCollision()->SetAsBox(50.0f, 200.0f);
	GetCollision()->SetComponentLocation(1400.0f, 800.0f);
}

EnemyUnitBase::~EnemyUnitBase()
{

}
