#include "pch.h"
#include "InputManager.h"
#include "Application.h"
namespace JG
{
	InputManager::InputManager()
	{
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::BeginSystem,
			[&]() -> EScheduleResult { 
			if (IWindow::GetFocus() == Application::GetInstance().GetWindow()->GetHandle())
			{
				RefreshInput();
			}
			return EScheduleResult::Continue;
		});
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Default, SCHEDULE_BIND_FN(&InputManager::Update));
	}
	ActionMappingData* InputManager::AddActionMappings(const String& name)
	{
		auto data = CreateSharedPtr<ActionMappingData>(name);
		mActionMappingsDataList.push_back(data);
		return data.get();
	}
	AxisMappingData* InputManager::AddAxisMappings(const String& name)
	{
		auto data = CreateSharedPtr<AxisMappingData>(name);
		mAxisMappingsDataList.push_back(data);
		return data.get();
	}
	void InputManager::RemoveActionMappings(ActionMappingData* data)
	{
		for (auto iter = mActionMappingsDataList.begin(); iter != mActionMappingsDataList.end();)
		{
			if ((*iter).get() == data)
			{
				iter = mActionMappingsDataList.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
	void InputManager::RemoveAxisMappings(AxisMappingData* data)
	{
		for (auto iter = mAxisMappingsDataList.begin(); iter != mAxisMappingsDataList.end();)
		{
			if ((*iter).get() == data)
			{
				iter = mAxisMappingsDataList.erase(iter);
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
	void InputManager::BindAction(IJGObject* _object, const String& actionName, EInputAction inputAction, const std::function<void()>& action, bool* pIsActive)
	{
		if (_object == nullptr)
		{
			return;
		}
		ActionBindData data;
		data.Func   = action;
		data.pIsActive = pIsActive;
		mActionBindedDatas[inputAction][actionName][_object] = data;
	}
	void InputManager::UnBindAction(IJGObject* _object, const String& actionName)
	{
		if (_object == nullptr)
		{
			return;
		}
		for (auto& _pair : mActionBindedDatas)
		{
			_pair.second[actionName].erase(_object);
		}
	}
	void InputManager::BindAxis(IJGObject* _object, const String& axisName,  const std::function<void(float)>& action, bool* pIsActive)
	{
		if (_object == nullptr)
		{
			return;
		}
		AxisBindData data;
		data.Func      = action;
		data.pIsActive = pIsActive;
		mAxisBindedDatas[axisName][_object] = data;
	}
	void InputManager::UnBindAxis(IJGObject* _object, const String& axisName)
	{
		if (_object == nullptr)
		{
			return;
		}
		mAxisBindedDatas[axisName].erase(_object);
	}
	void InputManager::SetCenterPointWhenHideCursor(const JVector2& centerPos)
	{
		mCursorCenterPoint = centerPos;
	}
	void InputManager::SetInputLock(bool isLock)
	{
		mIsLock = isLock;
	}
	bool InputManager::IsInputLock() const
	{
		return mIsLock;
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
	f32  InputManager::GetMouseDeltaX() const
	{
		return mMouseDelta.x;
	}
	f32  InputManager::GetMouseDeltaY() const
	{
		return mMouseDelta.y;
	}
	const JVector2& InputManager::GetMouseDelta() const
	{
		return mMouseDelta;
	}
	void InputManager::RefreshInput()
	{
		for (i32 i = 0; i < 256; ++i)
		{
			auto& state = mKeyState[i];
			if (state.State & KeyState::Released)
			{
				state.State = KeyState::None;
			}

			if (GetAsyncKeyState(i) & 0x08001)
			{
				if (state.State == KeyState::None)
				{
					state.State |= KeyState::Pressed;
					state.State |= KeyState::Down;
				}
				else
				{
					state.State = KeyState::Down;
				}
			}
			else
			{
				if (state.State & KeyState::Down)
				{
					state.State |= KeyState::Up;
					state.State |= KeyState::Released;
				}
			}
		}


		static bool isAxisInit = false;
		static JVector2 cursorPos;
		static JVector2 prevCursorPos;

		static bool isShowCursor = true;
		static JVector2 originCursorPos;

		if (isAxisInit == false)
		{
			isAxisInit = true;
			cursorPos	  = IWindow::GetCursorPos();
			prevCursorPos = cursorPos;
		}
		else
		{
			cursorPos  = IWindow::GetCursorPos();
			auto delta = cursorPos - prevCursorPos;
			SetAxisValue(EKeyCode::Mouse_X, delta.x);
			SetAxisValue(EKeyCode::Mouse_Y, delta.y);
			prevCursorPos = cursorPos;


			auto window = Application::GetInstance().GetWindow();
			if (window->IsShowCursor() == false)
			{
				if (isShowCursor == true)
				{
					originCursorPos = cursorPos;
					isShowCursor    = false;
				}
				
#ifdef JG_EDITOR
				JVector2 cursorCenter = mCursorCenterPoint - JVector2(window->GetPosition().x, window->GetPosition().y);
#else
				JRect    clientRect = IWindow::GetClientRect(window->GetHandle());
				JVector2 cursorCenter = clientRect.Center();
#endif // JG_EDITOR

				JVector2 centerPos  = IWindow::ClientToScreen(window->GetHandle(), cursorCenter);
				IWindow::SetCursorPos(centerPos);
				prevCursorPos = centerPos;
				cursorPos	  = centerPos;
			}
			else {
				if (isShowCursor == false)
				{
					isShowCursor = true;
					IWindow::SetCursorPos(originCursorPos);
				}
			}
		}


	}
	void InputManager::ActionMappingData_Update(EInputAction inputEvent, const String& mappingName)
	{
		auto bindDatas = mActionBindedDatas[inputEvent][mappingName];
		for (auto& _pair : bindDatas)
		{
			auto& data = _pair.second;
			if (data.pIsActive == nullptr || (*data.pIsActive) == true)
			{
				if (data.Func)
				{
					data.Func();
				}
			}
		}
	}
	void InputManager::AxisMappingData_Update(const String& mappingName, f32 value)
	{
		auto& bindDatas = mAxisBindedDatas[mappingName];
		for (auto& _pair : bindDatas)
		{
			auto& data = _pair.second;
			if (data.pIsActive == nullptr || (*data.pIsActive) == true)
			{
				if (data.Func)
				{
					data.Func(value);
				}
			}
		}
	}
	EScheduleResult InputManager::Update()
	{
		if (IsInputLock() == false) return EScheduleResult::Continue;
		for (auto& mappingData : mActionMappingsDataList)
		{
			for (auto& keyData : mappingData->KeyList)
			{
				if (keyData.Code == EKeyCode::Unknown) continue;
				bool isTrigger = true;
				if (keyData.IsAlt && IsKeyDown(EKeyCode::Alt) == false)
				{
					isTrigger = false;
				}
				if (keyData.IsCtrl && IsKeyDown(EKeyCode::Ctrl) == false)
				{
					isTrigger = false;
				}
				if (keyData.IsShift && IsKeyDown(EKeyCode::Shift) == false)
				{
					isTrigger = false;
				}

				if (isTrigger)
				{
					if (IsKeyPressed(keyData.Code) == true)
					{
						ActionMappingData_Update(EInputAction::Pressed, mappingData->Name);
					}
					if (IsKeyReleased(keyData.Code) == true)
					{
						ActionMappingData_Update(EInputAction::Released, mappingData->Name);
					}
				}
			}

		}
		for (auto& mappingData : mAxisMappingsDataList)
		{
			for (auto& keyData : mappingData->KeyList)
			{
				if (IsAxisKeyCode(keyData.Code) == true)
				{
					f32 value = GetAxisValue(keyData.Code);
					f32 deadZone = GetAxisDeadZone(keyData.Code);
					if (Math::Abs(value) >= deadZone)
					{
						AxisMappingData_Update(mappingData->Name, value * keyData.Scale);
					}
				}
				else
				{
					if (IsKeyDown(keyData.Code) == true)
					{
						AxisMappingData_Update(mappingData->Name, keyData.Scale);
					}
				}
			}


		}
		return EScheduleResult::Continue;
	}
	bool InputManager::IsAxisKeyCode(EKeyCode code) const
	{
		return code == EKeyCode::Mouse_X || code == EKeyCode::Mouse_Y;
	}
	void InputManager::SetAxisValue(EKeyCode code, f32 value)
	{
		if (IsAxisKeyCode(code))
		{
			mAxisDatas[code].Value = value;
		}
	}
	f32 InputManager::GetAxisValue(EKeyCode code) const
	{
		if (IsAxisKeyCode(code) && mAxisDatas.find(code) != mAxisDatas.end())
		{
			return mAxisDatas.at(code).Value;
		}
		return 0.0f;
	}
	f32 InputManager::GetAxisDeadZone(EKeyCode code) const
	{
		if (IsAxisKeyCode(code) && mAxisDatas.find(code) != mAxisDatas.end())
		{
			return mAxisDatas.at(code).DeadZone;
		}
		return 0.0f;
	}
}