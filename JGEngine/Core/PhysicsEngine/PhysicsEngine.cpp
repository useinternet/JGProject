#include "pch.h"
#include "PhysicsEngine.h"
#include "Class/Log.h"
using namespace std;

namespace PE
{
	PhysicsEngine::PhysicsEngine(const GlobalLinkStream& stream)
		: EngineCore(stream)
	{
		PE_LOG_INFO("PhysicsEngine Init Compelete...");
	}
	void PhysicsEngine::Update()
	{

	}
	void PhysicsEngine::OnEvent(Event& e)
	{

	}
}