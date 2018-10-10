#include"World.h"
#include"WorldManager.h"
#include"../../PhysicsSystem/JGBox2D/JGDynamics/JGPhysicsWorld.h"
#include"../../EngineStatics/JMath/JGMatrix.h"
#include"../Object/Object.h"
#include"../Object/GameMode.h"
using namespace std;
World::World(const std::wstring& Name, JGPhysicsWorld* pyWorld, WorldManager* manager)
{
	m_WorldManager = manager;
	m_pyWorld = pyWorld;


	m_ViewMatrix = make_unique<JGMatrix>();
	TempViewMatrixInit();
}
World::~World()
{

}
void World::Tick(const float DeltaTime)
{
	if (m_GameMode.get())
	{
		if (m_GameMode->IsGameEnd())
		{
			m_GameMode->GameEndEvent(DeltaTime);
			return;
		}
		m_GameMode->Tick(DeltaTime);
	}
	
	for (auto object = m_sObjects.begin(); object != m_sObjects.end();)
	{
		if ((*object)->GetObjectState() != EObjectState::Destory)
		{
			(*object)->Tick(DeltaTime);
			object++;
		}
		else
		{
			object = m_sObjects.erase(object);
		}
	}
	
}
list<shared_ptr<Object>>* World::GetObjectArray()
{
	return &m_sObjects;
}
void World::TempViewMatrixInit()
{
	JGVector3D up(0.0f, 1.0f, 0.0f), position(0.0f, 0.0f, -10.0f), lookAt(0.0f, 0.0f, 1.0f);
	JGMatrix rotationMatrix;

	rotationMatrix.MakeRotationMatirx(0.0f, 0.0f, 0.0f);
	lookAt.TransformCoord(rotationMatrix);
	up.TransformCoord(rotationMatrix);


	m_ViewMatrix->MakeViewMatrix(&position, &lookAt, &up);
}

GameMode* World::GetGameMode()
{
	return m_GameMode.get();
}

JGMatrix& World::GetViewMatrix()
{
	return *m_ViewMatrix;
}

JGPhysicsWorld* World::GetPyWorld()
{
	return m_pyWorld;
}
void World::Clear()
{
	m_sObjects.clear();
}
WorldManager* World::GetManager()
{
	return m_WorldManager;
}
