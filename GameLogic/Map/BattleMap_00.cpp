#include"BattleMap_00.h"
#include"EngineFrameWork/2D/Tile/Block.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"EngineFrameWork/2D/Tile/BackGround.h"
#include"EngineStatics/JGLog.h"
#include"EngineFrameWork/2D/Widget/ImageBox.h"
#include"EngineFrameWork/World/World.h"
#include"EngineFrameWork/Components/Camera2DComponent.h"
using namespace std;
BattlMap_00::BattlMap_00()
{
	RegisterObjectID(typeid(this));
	CullingIgnore();
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

	img = RegisterComponentInObject<ImageBox>(TT("BackGround"));
	img->CreateImage(TT("../Contents/BackGround/Sky_2.png"), EPivot::TopLeft);
	img->SetComponentScale(2.0f, 1.5f); img->SetZOrder(-1);
}
BattlMap_00::~BattlMap_00()
{


}
void BattlMap_00::BeginObject(World* world)
{
	ExistObject::BeginObject(world);
	
}
void BattlMap_00::Tick(const float DeltaTime)
{
	ExistObject::Tick(DeltaTime);
}

