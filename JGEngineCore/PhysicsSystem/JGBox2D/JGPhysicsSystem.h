#pragma once
#include"../../EngineStatics/Engine.h"
#include"../Box2D/Box2D.h"
class JGContactListener;
class JGCollisionDebugDraw;
class JGPhysicsWorld;
class JGVector2D;
class JGSuperClass;
class JGMatrix;
class ENGINE_EXPORT JGPhysicsSystem
{
private:
	std::map<const std::wstring, std::shared_ptr<JGPhysicsWorld>> m_mPhysicsWorlds;
	std::unique_ptr<JGCollisionDebugDraw> m_DebugDraw;
	std::unique_ptr<JGContactListener> m_Listener;

	std::wstring m_CurrentWorld = TT("None");
	int32 m_velocityIterations = 8;
	int32 m_positionIterations = 3;
public:
	JGPhysicsSystem();
	~JGPhysicsSystem();
	void InitPhysicsSystem(HWND hWnd,JGSuperClass* super);
	void DebugDrawSetViewMatrix(const JGMatrix& ViewMatrix);
	void SetCurrentPhysicsWorld(const std::wstring& WorldName);
	JGPhysicsWorld* AddPhysicsWorld(const std::wstring& WorldName,JGVector2D& Gravity);
	void Step(const float DeltaTime);
	void Render();
};