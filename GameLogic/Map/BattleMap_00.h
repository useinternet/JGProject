#pragma once
#include"EngineFrameWork/Object/ExistObject.h"
#include"StaticFilter/StaticCollisionFilter.h"
class BattlMap_00 : public ExistObject
{
	const int GroundFilter = Filter_Enemy | Filter_Item | Filter_PlayerCollision;
private:
	class Block* BasicBlock;
	class Block* LeftWall;
	class ImageBox* img;
public:
	BattlMap_00();
	virtual ~BattlMap_00();
	virtual void BeginObject(World* world) override;
	virtual void Tick(const float DeltaTime) override;
};