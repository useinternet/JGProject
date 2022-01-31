#include "GameObjectFactory.h"
#include "pch.h"
#include "GameObjectFactory.h"
#include "Application.h"
#include "Graphics/JGGraphics.h"
#include "Event/GameEvent.h"

namespace JG
{
	Type GameObjectFactory::GetGameObjectType(const String& typeName) const
	{
		if (mObjectTypeByName.find(typeName) == mObjectTypeByName.end())
		{
			return Type();
		}
		return mObjectTypeByName.at(typeName);
	}
	Type GameObjectFactory::GetGameObjectType(u64 typeID) const
	{
		if (mObjectTypeByID.find(typeID) == mObjectTypeByID.end())
		{
			return Type();
		}
		return mObjectTypeByID.at(typeID);
	}

	bool GameObjectFactory::IsGameComponent(const Type& type) const
	{
		return mRegisteredComponentTypeSet.find(type) != mRegisteredComponentTypeSet.end();
	}
	bool GameObjectFactory::IsGameNode(const Type& type)      const
	{
		return mRegisteredNodeTypeSet.find(type) != mRegisteredNodeTypeSet.end();
	}
	bool GameObjectFactory::IsGameSystem(const Type& type)    const
	{
		return mRegisteredSystemTypeSet.find(type) != mRegisteredSystemTypeSet.end();
	}
	bool GameObjectFactory::IsGlobalGameSystem(const Type& type) const
	{
		return mRegisteredGlobalSystemTypeSet.find(type) != mRegisteredGlobalSystemTypeSet.end();
	}
	bool GameObjectFactory::IsGameComponent(IGameObject* gameObject) const
	{
		return IsGameComponent(gameObject->GetType());
	}
	bool GameObjectFactory::IsGameNode(IGameObject* gameObject) const
	{
		return IsGameNode(gameObject->GetType());
	}
	bool GameObjectFactory::IsGameSystem(IGameObject* gameObject) const
	{
		return IsGameSystem(gameObject->GetType());
	}
	bool GameObjectFactory::IsGlobalGameSystem(IGameObject* gameObject) const
	{
		return IsGlobalGameSystem(gameObject->GetType());
	}
	IGameObject* GameObjectFactory::CreateObjectByType(const Type& type)
	{
		if (mCreateFuncByObjectType.find(type) == mCreateFuncByObjectType.end())
		{
			return nullptr;
		}
		return mCreateFuncByObjectType[type]();
	}
	const HashSet<Type>& GameObjectFactory::GetGameNodeTypeSet() const
	{
		return mRegisteredNodeTypeSet;
	}
	const HashSet<Type>& GameObjectFactory::GetGameComponentTypeSet() const
	{
		return mRegisteredComponentTypeSet;
	}
	void GameObjectFactory::CreateObjectImpl(IGameObject* gameObject)
	{

	}
	void GameObjectFactory::DestroyObjectImpl(IGameObject* gameObject)
	{

	}
	void GameObjectFactory::ReserveDestroyObjectImpl(IGameObject* gameObject)
	{
		bool is_run_destroy = mReservedDestroyObjects.empty() == false;

		mReservedDestroyObjects.push_back(gameObject);

		if (is_run_destroy == false)
		{
			Scheduler::GetInstance().ScheduleOnceByFrame(1, SchedulePriority::DestroyGameClass, SCHEDULE_BIND_FN(&GameObjectFactory::UpdateDestroyObject));
		}
		NotifyDestroyJGObjectEvent e;
		e.DestroyedObject = gameObject;
		Application::GetInstance().SendEvent(e);
	}
	EScheduleResult GameObjectFactory::UpdateDestroyObject()
	{
		for (auto& obj : mReservedDestroyObjects)
		{
			JG_LOG_INFO("GameNode : {0}  Destroy", obj->GetName());
			obj->Destory();
		}
		mReservedDestroyObjects.clear();
		return EScheduleResult::Continue;
	}

}