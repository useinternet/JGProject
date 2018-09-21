#pragma once
#pragma once
#include"../../Box2D/Box2D.h"
#include"../../../EngineStatics/JMath/JGVector2D.h"
#include"../../../EngineStatics/JMath/JGAngle2D.h"
//임시 크기
#define PTM 108
static float g_ScreenWidth = 1920;
static float g_ScreenHeight = 1080;


inline b2Vec2 JGTob2_Vector(JGVector2D& vec)
{
	b2Vec2 result;
	result.x = (vec.X() - (g_ScreenWidth / 2)) / PTM;
	result.y = (-vec.Y() + (g_ScreenHeight / 2)) / PTM;

	return result;
}
inline JGVector2D b2ToJG_Vector(b2Vec2& vec)
{
	JGVector2D result;
	result.Set(
		(vec.x * PTM) + (g_ScreenWidth / 2),
		-((vec.y * PTM) - (g_ScreenHeight / 2)));
	return result;
}
inline b2Vec2 JGTob2_Unit(JGVector2D& vec)
{
	b2Vec2 result;
	result.x = vec.X() / PTM;
	result.y = vec.Y() / PTM;

	return result;
}
inline JGVector2D b2ToJG_Unit(b2Vec2& vec)
{
	JGVector2D result;
	result.Set(vec.x*PTM, vec.y * PTM);
	return result;
}
inline b2Vec2 JGTob2_Force(JGVector2D& vec)
{
	b2Vec2 result;
	result.x = vec.X() / PTM;
	result.y = -vec.Y() / PTM;

	return result;
}

inline JGVector2D b2ToJG_Force(b2Vec2& vec)
{
	JGVector2D result;
	result.Set(vec.x* PTM, -vec.y * PTM);

	return result;
}