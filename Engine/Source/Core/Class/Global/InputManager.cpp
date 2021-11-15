#include "pch.h"
#include "InputManager.h"

namespace JG
{
	InputManager::InputManager()
	{
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::BeginSystem,
			[&]() -> EScheduleResult { 
			Update(); 
			return EScheduleResult::Continue;
		});
	}
	void InputManager::AddActionMappings(const String& name)
	{
		if (mActionMappingsDataDic.find(name) == mActionMappingsDataDic.end())
		{
			auto data = CreateSharedPtr<ActionMappingData>(name);
			mActionMappingsDataList.push_back(data);
			mActionMappingsDataDic[name] = data.get();
		}
	}
	void InputManager::AddAxisMappings(const String& name)
	{
		if (mAxisMappingsDataDic.find(name) == mAxisMappingsDataDic.end())
		{
			auto data = CreateSharedPtr<AxisMappingData>(name);
			mAxisMappingsDataList.push_back(data);
			mAxisMappingsDataDic[name] = data.get();
		}
	}
	void InputManager::RemoveActionMappings(const String& name)
	{
		for (auto iter = mActionMappingsDataList.begin(); iter != mActionMappingsDataList.end();)
		{
			if ((*iter)->Name == name)
			{
				iter = mActionMappingsDataList.erase(iter);
				mActionMappingsDataDic.erase(name);
			}
			else
			{
				++iter;
			}
		}
	}
	void InputManager::RemoveAxisMappings(const String& name)
	{
		for (auto iter = mAxisMappingsDataList.begin(); iter != mAxisMappingsDataList.end();)
		{
			if ((*iter)->Name == name)
			{
				iter = mAxisMappingsDataList.erase(iter);
				mAxisMappingsDataDic.erase(name);
			}
			else
			{
				++iter;
			}
		}
	}
	void InputManager::ForEach(const std::function<void(ActionMappingData*)>& action)
	{
		for (auto& data : mActionMappingsDataList)
		{
			action(data.get());
		}
	}
	void InputManager::ForEach(const std::function<void(AxisMappingData*)>& action)
	{
		for (auto& data : mAxisMappingsDataList)
		{
			action(data.get());
		}
	}
	bool InputManager::IsKeyPressed(EKeyCode code)
	{
		return mKeyState[(i32)code].State & KeyState::Pressed;
	}
	bool InputManager::IsKeyReleased(EKeyCode code)
	{
		return mKeyState[(i32)code].State & KeyState::Released;
	}
	bool InputManager::IsKeyDown(EKeyCode code)
	{
		return mKeyState[(i32)code].State & KeyState::Down;
	}
	bool InputManager::IsKeyUp(EKeyCode code)
	{
		return mKeyState[(i32)code].State & KeyState::Up;
	}
	void InputManager::Update()
	{
		for (i32 i = 0; i < 256; ++i)
		{
			auto& state = mKeyState[i];
			if (GetAsyncKeyState(i) & 0x08001)
			{
				if (state.State == KeyState::None)
				{
					state.State |= KeyState::Pressed;
					state.State |= KeyState::Down;
					JG_CORE_INFO("KeyPressed : {0}", KeyCodeToString((EKeyCode)i));
				}
				else
				{
					state.State = KeyState::Down;
					JG_CORE_INFO("KeyDown : {0}", KeyCodeToString((EKeyCode)i));
				}
			}
			else
			{
				if (state.State & KeyState::Down)
				{
					state.State |= KeyState::Up;
					state.State |= KeyState::Released;
					JG_CORE_INFO("KeyUp : {0}", KeyCodeToString((EKeyCode)i));
					JG_CORE_INFO("KeyReleased : {0}", KeyCodeToString((EKeyCode)i));
				}
				if (state.State & KeyState::Up)
				{
					state.State = KeyState::None;
				}
			}
		}
	}
}