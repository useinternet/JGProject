#include "pch.h"
#include "DX12_Scene.h"
#include "DX12_Texture.h"
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
		m_SceneObjs.push_back(sceneObject);
	}

	void DX12_Scene::PushObjects(const vector<GE::SceneObject*>& sceneObject)
	{
		m_SceneObjs.insert(m_SceneObjs.end(), sceneObject.begin(), sceneObject.end());
	}

	void DX12_Scene::FlushObjects()
	{
		m_SceneObjs.clear();
	}


	GE::SceneCamera* DX12_Scene::GetBindedCamera() const
	{
		TextureSetting();
		return m_Cam;
	}

	std::vector<GE::SceneObject*> DX12_Scene::GetPushedObjects() const
	{
		// 정렬 이나 카메라 컬링 후 배출
		return m_SceneObjs;
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

