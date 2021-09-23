#include "pch.h"
#include "GameObject.h"
#include "GameComponent.h"
#include "GameNode.h"
#include "GameWorld.h"
#include "Components/Transform.h"
#include "Class/Global/Scheduler.h"

namespace JG
{
	GameObject::~GameObject()
	{
		for (auto& handle : mScheduleHandleList)
		{
			handle->Reset();
		}
		mScheduleHandleList.clear();
	}
	SharedPtr<ScheduleHandle> GameObject::Schedule(f32 delay, f32 tickCycle, i32 repeat, i32 priority, const SyncTaskFunction& task)
	{
		ArrangeScheduleHandle();
		auto handle = Scheduler::GetInstance().Schedule(delay, tickCycle, repeat, SchedulePriority::UpdateGameClass + priority, task);
		mScheduleHandleList.push_back(handle);
		return handle;
	}
	SharedPtr<ScheduleHandle> GameObject::ScheduleOnce(f32 delay, i32 priority, const SyncTaskFunction& task)
	{
		ArrangeScheduleHandle();
		auto handle = Scheduler::GetInstance().ScheduleOnce(delay, SchedulePriority::UpdateGameClass + priority, task);
		mScheduleHandleList.push_back(handle);
		return handle;
	}
	SharedPtr<ScheduleHandle> GameObject::ScheduleByFrame(i32 delayFrame, i32 frameCycle, i32 repeat, i32 priority, const SyncTaskFunction& task)
	{
		ArrangeScheduleHandle();
		auto handle = Scheduler::GetInstance().ScheduleByFrame(delayFrame, frameCycle, repeat, SchedulePriority::UpdateGameClass + priority, task);
		mScheduleHandleList.push_back(handle);
		return handle;
	}
	SharedPtr<ScheduleHandle> GameObject::ScheduleOnceByFrame(i32 delayFrame, i32 priority, const SyncTaskFunction& task)
	{
		ArrangeScheduleHandle();
		auto handle = Scheduler::GetInstance().ScheduleOnceByFrame(delayFrame, SchedulePriority::UpdateGameClass + priority, task);
		mScheduleHandleList.push_back(handle);
		return handle;
	}
	SharedPtr<ScheduleHandle> GameObject::ScheduleAsync(const AsyncTaskFunction& task)
	{
		return Scheduler::GetInstance().ScheduleAsync(task);
	}
	void GameObject::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		jsonData->AddMember("Name", GetName());

	}
	void GameObject::LoadJson(SharedPtr<JsonData> jsonData)
	{
		auto val = jsonData->GetMember("Name");
		if (val)
		{
			SetName(val->GetString());
		}
	}
	const String& GameObject::GetName() const
	{
		if (mName.empty())
		{
			mName = GetType().GetName();
			u64 pos = mName.find("JG::");
			if (pos != String::npos)
			{
				mName = mName.substr(pos + 4);
			}
		}

		return mName;
	}
	void GameObject::SetName(const String& name)
	{
		mName = name;
	}
	void GameObject::ArrangeScheduleHandle()
	{
		u64 cnt = mScheduleHandleList.size();
		for (u64 i = 0; i < cnt;)
		{
			if (mScheduleHandleList[i]->IsValid() == false)
			{
				std::swap(mScheduleHandleList.end() - 1, mScheduleHandleList.begin() + i);
				mScheduleHandleList.pop_back();
				--cnt;
			}
			else
			{
				++i;
			}
		}
	}
	void GameObject::DestoryObject(GameComponent* component)
	{
		if (component == nullptr)
		{

			return;
		}
		auto owner = component->GetOwner();
		if (owner == nullptr)
		{
			return;
		}
		if (owner->mTransform == component)
		{
			return;
		}
		auto& begin = owner->mComponents.begin();
		auto& end = owner->mComponents.end();
		owner->mComponents.erase(std::remove(begin, end, component), owner->mComponents.end());
		GameObjectFactory::GetInstance().DestroyObject(component);
	}
	void GameObject::DestoryObject(GameNode* gameNode)
	{
		if (gameNode == nullptr)
		{
			return;
		}
		gameNode->DestroyRecursive();
	}
	void GameObject::DestoryObject(GameWorld* gameWorld)
	{
		if (gameWorld == nullptr)
		{
			return;
		}
		gameWorld->DestroyRecursive();
		for (auto& sys : gameWorld->mWorldGameSystemList)
		{
			DestoryObject(sys);
		}
	}
	void GameObject::DestoryObject(GameSystem* gameSys)
	{
		if (gameSys == nullptr)
		{
			return;
		}
		auto owner = gameSys->GetOwnerGameWorld();
		if (owner == nullptr)
		{
			return;
		}

		auto& begin = owner->mWorldGameSystemList.begin();
		auto& end = owner->mWorldGameSystemList.end();
		owner->mWorldGameSystemList.erase(std::remove(begin, end, gameSys), owner->mWorldGameSystemList.end());
		GameObjectFactory::GetInstance().DestroyObject(gameSys);
	}
}

