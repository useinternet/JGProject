#pragma once
#include"EngineFrameWork/AI/BT_InformationBoard.h"
#include"EngineFrameWork/Object/Object.h"
class BTIB_EnemyBoard : public BT_InformationBoard
{
public:
	BTIB_EnemyBoard();
	~BTIB_EnemyBoard();


	Object* EnemyObject = nullptr;
};