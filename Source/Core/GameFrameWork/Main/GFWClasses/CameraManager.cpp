#include "pch.h"
#include "CameraManager.h"
#include "GFWIF.h"
#include "World.h"



using namespace std;


namespace GFW
{




	void CameraManager::Awake()
	{
		GFWIF::RegisterInstanceWithGraphicsEngine(this);
		m_WorldCamera = make_unique<Camera>();
		m_WorldCamera->scene = GFWIF::Request3DSceneWithGraphicsEngine(this);
		m_WorldCamera->camera.SetLens(0.45f * JG_PI, 1920, 1080, 1000000.0f, 0.1f);
		m_WorldCamera->camera.SetPosition({ 0,0,-100 });
		m_WorldCamera->scene->BindCamera(&m_WorldCamera->camera);
	}

	void CameraManager::Tick()
	{
		if (GFWIF::GetGameWorld())
		{
			auto sceneObjs = GFWIF::GetGameWorld()->GetSceneObjects();


			if (m_WorldCamera->Enable)
			{
				m_WorldCamera->scene->PushObjects(sceneObjs);
				GraphicsIF::DrawCall(m_WorldCamera->scene);
			}
		}
	}

	void CameraManager::Destroy()
	{
		GFWIF::UnRegisterInstanceWithGraphicsEngine(this);
	}

}