#pragma once
#include"../../EngineStatics/Engine.h"

class PhysicsWorld;
class JGShape;
class JG2DBody;
enum class EShapeType;
class ENGINE_EXPORT JGCollisionCheckManager
{
private:
	PhysicsWorld* m_pPhysicsWorld = nullptr;
public:
	JGCollisionCheckManager() = delete;
	JGCollisionCheckManager(PhysicsWorld* world);
	~JGCollisionCheckManager();

	void Tick();
private:
	bool CollisionCheck(JG2DBody* Body1, JG2DBody* Body2);
	bool RectToRectCollisionCheck(JGShape* Rect1, JGShape* Rect2);
	bool RectToDotCollisionCheck();
	bool RectToLineCollisionCheck();
	bool RectToCircleCollisionCheck();

};