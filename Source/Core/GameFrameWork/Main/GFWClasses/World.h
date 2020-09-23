#pragma once
#include "GFWObject.h"





namespace GFW
{

	class SubGameWorld;
	class GameObject;
	class GAMEFRAMEWORK_API GameWorld : public GFWObject
	{
		friend class GFWIF;
	public:
		GameWorld(const std::wstring& name) : GFWObject(name) {}
		virtual ~GameWorld() {}
	protected:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;

		// OnCreate
		// OnStart
	public:
		SubGameWorld* GetSelectedWorld() const;
		std::vector<GE::SceneObject*> GetSceneObjects() const;
	private:
		// 월드 관리
		std::unique_ptr<SubGameWorld>              m_MainWorld = nullptr;
		std::unordered_map<std::wstring, std::unique_ptr<SubGameWorld>> m_SubWorlds;

		SubGameWorld* m_SelectedWorld = nullptr;
		bool m_IsExecuteStart = false;


		// 에디터 카메라 관리
	};

	class GAMEFRAMEWORK_API SubGameWorld : public GFWObject
	{
		friend GameWorld;
	public:
		SubGameWorld(const std::wstring& name) : GFWObject(name) {}
		virtual ~SubGameWorld() {}
	protected:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;
	public:
		void SetVisible(bool visible) {
			m_IsVisible = visible;
		}
		bool IsVisible() const {
			return m_IsVisible;
		}
	public:
		template<class GameObjectType>
		GameObjectType* SpawnGameObject(const std::wstring& name) {
			auto obj = std::make_shared< GameObjectType>(name);
			obj->m_OwnerWorld = this;
			obj->Awake();

			m_GameObjectPool[obj.get()] = obj;

			return obj.get();
		}
		void DestroyGameObject(GameObject* object);

		void PushSceneObjects(const std::vector<GE::SceneObject*>& obj);
		void PopSceneObjects();
	private:
		using GameObjectPool = std::unordered_map<GameObject*, std::shared_ptr<GameObject>>;

		GameWorld*                     m_OwnerWorld = nullptr;
		GameObjectPool                 m_GameObjectPool;
		std::queue<GameObject*>        m_DestroyGameObjectQueue;
		std::vector<GE::SceneObject*>  m_SceneObjects;
		bool m_IsVisible = true;
		bool m_IsExecuteStart = false;
	};
}