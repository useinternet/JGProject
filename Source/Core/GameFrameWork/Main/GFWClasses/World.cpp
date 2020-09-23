#include "pch.h"
#include "World.h"
#include "GameObject.h"



using namespace std;
namespace GFW
{





	void GameWorld::Awake()
	{
		m_MainWorld = make_unique<SubGameWorld>(TT("Main"));
		m_MainWorld->m_OwnerWorld = this;
		m_SelectedWorld = m_MainWorld.get();


		/*
		* GUI = Detail
		* 
		
		*/


		/* 에디터용 월드 카메라 만들기
		*/
		//m_MainWorld->SpawnGameObject(TT("ViewportEditorCamera"));

	}

	void GameWorld::Start()
	{
	}

	void GameWorld::Tick()
	{
		if (!m_IsExecuteStart)
		{
			m_IsExecuteStart = true;
			Start();
		}


		m_MainWorld->PopSceneObjects();
		m_MainWorld->Tick();

		for (auto& subWorld : m_SubWorlds)
		{
			subWorld.second->PopSceneObjects();
			subWorld.second->Tick();
		}
		


	}

	void GameWorld::Destroy()
	{
	}

	SubGameWorld* GameWorld::GetSelectedWorld() const
	{
		return m_SelectedWorld;
	}

	std::vector<GE::SceneObject*> GameWorld::GetSceneObjects() const
	{
		std::vector<GE::SceneObject*> result;
		if (m_MainWorld->IsVisible())
		{
			result.insert(result.end(), m_MainWorld->m_SceneObjects.begin(), m_MainWorld->m_SceneObjects.end());
		}

		for (auto& world : m_SubWorlds)
		{
			if(world.second->IsVisible())
			{
				result.insert(result.end(), world.second->m_SceneObjects.begin(), world.second->m_SceneObjects.end());
			}
		}


		return result;
	}




}
namespace GFW 
{
	void SubGameWorld::Awake()
	{
	}
	void SubGameWorld::Start()
	{
	}
	void SubGameWorld::Tick()
	{
		if (!m_IsExecuteStart)
		{
			m_IsExecuteStart = true;
			Start();
		}


		for (auto& obj : m_GameObjectPool)
		{
			obj.second->Tick();
		}

		while (!m_DestroyGameObjectQueue.empty())
		{
			auto obj = m_DestroyGameObjectQueue.front(); m_DestroyGameObjectQueue.pop();
			obj->Destroy();
			m_GameObjectPool.erase(obj);

		}
	}
	void SubGameWorld::Destroy()
	{


	}
	void SubGameWorld::DestroyGameObject(GameObject* object)
	{
		m_DestroyGameObjectQueue.push(object);
	}
	void SubGameWorld::PushSceneObjects(const std::vector<GE::SceneObject*>& obj)
	{
		m_SceneObjects.insert(m_SceneObjects.end(), obj.begin(), obj.end());
	}
	void SubGameWorld::PopSceneObjects()
	{
		m_SceneObjects.clear();
	}
}