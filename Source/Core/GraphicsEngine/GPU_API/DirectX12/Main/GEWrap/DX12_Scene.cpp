#include "pch.h"
#include "DX12_Scene.h"
#include "DX12_Texture.h"
#include "DX12_SceneObject.h"
#include "DX12_SceneLight.h"
using namespace std;



namespace DX12
{
	DX12_Scene::DX12_Scene() : GE::Scene()
	{
		m_SceneTexture = make_unique<DX12_Texture>();
	}
	void DX12_Scene::BindCamera(GE::SceneCamera* cam)
	{
		m_Cam = cam;

		TextureSetting();

	}

	void DX12_Scene::UnBindCamera()
	{
		m_Cam = nullptr;
		
	}


	void DX12_Scene::PushObject(GE::SceneObject* sceneObject)
	{
		if (sceneObject->GetMaterial())
		{
			auto& ppt = sceneObject->GetMaterial()->GetOwnerMaterial()->GetMaterialProperty();
			if (ppt.rendererMode != GetRendererMode())
			{
				GELOG_ERROR("Scene의 렌더러 모드와 Material의 렌더러모드가 일치 하지 않습니다.");
				return;
			}
		}
		m_SceneObjs.push_back(sceneObject);
	}

	void DX12_Scene::PushObjects(const vector<GE::SceneObject*>& sceneObject)
	{
		m_SceneObjs.insert(m_SceneObjs.end(), sceneObject.begin(), sceneObject.end());

		for (auto& iter = m_SceneObjs.begin(); iter < m_SceneObjs.end();)
		{
			if ((*iter)->GetMaterial())
			{
				auto& ppt = (*iter)->GetMaterial()->GetOwnerMaterial()->GetMaterialProperty();
				if (ppt.rendererMode != GetRendererMode())
				{
					GELOG_ERROR("Scene의 렌더러 모드와 Material의 렌더러모드가 일치 하지 않습니다.");
					iter = m_SceneObjs.erase(iter);
				}
				else ++iter;
			}
		}
	}

	void DX12_Scene::PushLight(GE::SceneLight* light)
	{
		switch (light->GetType())
		{
		case GE::LightType::Directional:
			m_DirectionalLight = static_cast<GE::DirectionalLight*>(light);
			break;
		case GE::LightType::Point:
			m_PointLights.push_back(static_cast<GE::PointLight*>(light));
			break;
		case GE::LightType::Spot:
			m_SpotLights.push_back(static_cast<GE::SpotLight*>(light));
			break;
		}

	}

	void DX12_Scene::FlushObjects()
	{
		m_DirectionalLight = nullptr;
		m_SceneObjs.clear();
		m_PointLights.clear();
		m_SpotLights.clear();
	}


	GE::SceneCamera* DX12_Scene::GetBindedCamera() const
	{
		TextureSetting();
		return m_Cam;
	}

	const std::vector<GE::SceneObject*>& DX12_Scene::GetPushedObjects() const
	{
		// 정렬 이나 카메라 컬링 후 배출
		return m_SceneObjs;
	}
	GE::DirectionalLight* DX12_Scene::GetPushedDirectionalLight() const
	{
		return m_DirectionalLight;
	}
	const std::vector<GE::PointLight*>& DX12_Scene::GetPushedPointLights() const
	{
		return m_PointLights;
	}
	const std::vector<GE::SpotLight*>& DX12_Scene::GetPushedSpotLights() const
	{
		return m_SpotLights;
	}
	GE::Texture* DX12_Scene::GetSceneTexture() const
	{
		TextureSetting();
		return m_SceneTexture.get();
	}
	DX12_Scene::~DX12_Scene()
	{
		m_Cam = nullptr;
		m_SceneObjs.clear();
		m_SceneTexture.reset();

	}
	void DX12_Scene::TextureSetting() const
	{
		DX12::Texture* t = (DX12::Texture*)m_SceneTexture->GetUserTextureData();
		lock_guard<mutex> lock(m_TMutex);
		if (t->IsValid())
		{
			t->Resize(m_Cam->GetCameraCB().lensWidth, m_Cam->GetCameraCB().lensHeight);
		}
		else
		{
			t->CreateRenderTargetTexture(TT("SceneTexture"), DXGI_FORMAT_R8G8B8A8_UNORM,
				m_Cam->GetCameraCB().lensWidth, m_Cam->GetCameraCB().lensHeight, 1, 1, { 1.0f,0.0f,0.0f,1.0f });
		}
	}
}

