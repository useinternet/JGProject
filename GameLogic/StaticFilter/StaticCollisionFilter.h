#pragma once
enum CollisionFilter
{
	Filter_None            = 0x00000001,
	Filter_PlayerCollision = 0x00000002,
	Filter_PlayerAttack    = 0x00000004,
	Filter_Ground          = 0x00000008,
	Filter_Enemy           = 0x00000016,
	Filter_EnemyAttack     = 0x00000032,
	Filter_Item            = 0x00000064
};