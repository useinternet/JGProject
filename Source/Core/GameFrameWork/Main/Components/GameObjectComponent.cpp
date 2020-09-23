#include "pch.h"
#include "GameObjectComponent.h"



namespace GFW
{
	void GameObjectComponent::Awake()
	{
	}

	void GameObjectComponent::Start()
	{
	}
	void GameObjectComponent::Tick()
	{
		if (!m_IsExecuteStart) {
			Start();
			m_IsExecuteStart = true;
		}
	}
	void GameObjectComponent::Destroy()
	{
	}
}

