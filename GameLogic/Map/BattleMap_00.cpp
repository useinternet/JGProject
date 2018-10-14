#include"BattleMap_00.h"
#include"EngineFrameWork/2D/Tile/Block.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"



using namespace std;
BattlMap_00::BattlMap_00()
{
	RegisterObjectID(typeid(this));
	BasicBlock = RegisterComponentInObject<Block>(TT("BasicBlock"));
	BasicBlock->CreateBlock(TT("../Contents/Ground/ground_green.png"), 150,1);
	BasicBlock->SetComponentLocation(-100.0f, 900.0f);
	BasicBlock->GetCollision()->AddMaskFilter(GroundFilter);
	BasicBlock->GetCollision()->SetCategoryFilter(Filter_Ground);

	LeftWall = RegisterComponentInObject<Block>(TT("LeftWall"));
	LeftWall->CreateBlock(TT("../Contents/Ground/ground_stone.png"), 1, 50);
	LeftWall->SetComponentLocation(-100.0f, -400.0f);
	LeftWall->GetCollision()->AddMaskFilter(GroundFilter);
	LeftWall->GetCollision()->SetCategoryFilter(Filter_Ground);
}
BattlMap_00::~BattlMap_00()
{


}

