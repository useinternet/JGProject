#pragma once
#include"../../EngineStatics/Engine.h"

class PhysicsWorld;
class JGShape;
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
	bool CollisionCheck(JGShape* Shape1, JGShape* Shape2);
	bool RectToRectCollisionCheck(JGShape* Rect1, JGShape* Rect2);
	bool RectToDotCollisionCheck();
	bool RectToLineCollisionCheck();
	bool RectToCircleCollisionCheck();

};