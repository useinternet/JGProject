#include"JGPhysicsSystem.h"
#include"JGDynamics/JGPhysicsWorld.h"
#include"JGCommon/JGCollisionDebugDraw.h"
#include"../../EngineStatics/JGSuperClass.h"
#include"../../EngineStatics/JMath/JGMatrix.h"
using namespace std;
JGPhysicsSystem::JGPhysicsSystem()
{
	m_DebugDraw = make_unique<JGCollisionDebugDraw>();
	//m_DebugDraw->InitCollisionDebugDraw()
}

JGPhysicsSystem::~JGPhysicsSystem()
{
}

void JGPhysicsSystem::InitPhysicsSystem(HWND hWnd,JGSuperClass* super)
{
	m_DebugDraw->InitCollisionDebugDraw(
		super->GetDevice(), hWnd, super->GetHLSLDevice(), super->GetBufferManager(),
		super->GetViewport());
}

void JGPhysicsSystem::DebugDrawSetViewMatrix(const JGMatrix& ViewMatrix)
{
	m_DebugDraw->SetViewMatrix(ViewMatrix);
}

void JGPhysicsSystem::SetCurrentPhysicsWorld(const wstring& WorldName)
{
	m_CurrentWorld = WorldName;
}

JGPhysicsWorld* JGPhysicsSystem::AddPhysicsWorld(const wstring& WorldName,JGVector2D& Gravity)
{
	JGPhysicsWorld* result = nullptr;
	unique_ptr<JGPhysicsWorld> pyWorld = make_unique<JGPhysicsWorld>(m_DebugDraw.get(),Gravity);
	result = pyWorld.get();
	m_mPhysicsWorlds.insert(pair <const wstring, shared_ptr<JGPhysicsWorld>>(WorldName, move(pyWorld)));
	return result;
}

void JGPhysicsSystem::Step(const float DeltaTime)
{
	if (m_CurrentWorld == TT("None"))
	{
		return;
	}
	m_mPhysicsWorlds[m_CurrentWorld]->Step(DeltaTime, m_velocityIterations, m_positionIterations);
}

void JGPhysicsSystem::Render()
{
	m_mPhysicsWorlds[m_CurrentWorld]->DebugRender();
}


