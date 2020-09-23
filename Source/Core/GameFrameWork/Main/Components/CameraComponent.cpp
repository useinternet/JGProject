#include "pch.h"
#include "CameraComponent.h"
#include "GFWIF.h"
#include "GFWClasses/GameObject.h"

using namespace std;

namespace GFW
{



	void CameraComponent::Awake()
	{
		HierarchicalComponent::Awake();
		m_SceneCamera.SetLens(0.45f * JG_PI, 1920, 1080, 100000000.0f, 0.1f, false);

		GFWIF::RegisterInstanceWithGraphicsEngine(this);
	}

	void CameraComponent::Start()
	{
		HierarchicalComponent::Start();

	}

	void CameraComponent::Tick()
	{
		HierarchicalComponent::Tick();

		m_SceneCamera.SetPosition(GetWorldLocation());
		m_SceneCamera.SetRotation(GetWorldRotation());
	}

	void CameraComponent::Destroy()
	{
		HierarchicalComponent::Destroy();
		GFWIF::UnRegisterInstanceWithGraphicsEngine(this);
	}

}