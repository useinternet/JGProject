#pragma once
#include "JGCore.h"

#define INPUT_ACTION_BIND(func) std::bind(func, this)
#define INPUT_AXIS_BIND(func) std::bind(func, this, std::placeholders::_1)
namespace JG
{
	enum class EScheduleResult;
	struct ActionMappingData
	{
		struct KeyData
		{
			EKeyCode Code =  EKeyCode::Unknown;
			bool IsShift = false;
			bool IsCtrl  = false;
			bool IsAlt   = false;
		};
		String Name;
		List<KeyData> KeyList;

		ActionMappingData() = default;
		ActionMappingData(const String& name) : Name(name) {}
	};
	struct AxisMappingData
	{
		struct KeyData
		{
			EKeyCode Code = EKeyCode::Unknown;
			f32 Scale = 0.0f;
		};

		String Name;
		List<KeyData> KeyList;

		AxisMappingData() = default;
		AxisMappingData(const String & name) : Name(name) {}
	};

	class InputManager : public GlobalSingleton<InputManager>
	{
		friend class Application;

	private:
		struct ActionBindData
		{
			std::function<void()> Func;
			bool* pIsActive = nullptr;
		};
		struct AxisBindData
		{
			std::function<void(float)> Func;
			bool* pIsActive = nullptr;
		};
		struct AxisData
		{
			f32 DeadZone = 0.0f;
			f32 Value    = 0.0f;
		};
		struct KeyState
		{
			enum
			{
				None, 
				Down = 0x0001,
				Up   = 0x0002,
				Pressed = 0x0004,
				Released = 0x0008
			};
			i32 State     = None;
			f32 PressDurationTime = 0.0f;
		};
		using ActionBindDatas = Dictionary<EInputAction, Dictionary<String, Dictionary<IJGObject*, ActionBindData>>>;
		using AxisBindDatas   = Dictionary<String, Dictionary<IJGObject*, AxisBindData>>;

		KeyState mKeyState[256];
		Dictionary<EKeyCode, AxisData>	   mAxisDatas;
		List<SharedPtr<ActionMappingData>> mActionMappingsDataList;
		List<SharedPtr<AxisMappingData>>   mAxisMappingsDataList;
		ActionBindDatas	mActionBindedDatas;
		AxisBindDatas   mAxisBindedDatas;
		JVector2		mMouseDelta;
		JVector2 mCursorCenterPoint;
		bool mIsLock = false;
	public:
		InputManager();
		virtual ~InputManager() = default;
	public:
		ActionMappingData* AddActionMappings(const String& name);
		AxisMappingData*   AddAxisMappings(const String& name);

		void RemoveActionMappings(ActionMappingData* data);
		void RemoveAxisMappings(AxisMappingData* data);

		void ForEach(const std::function<void(ActionMappingData*)>& action);
		void ForEach(const std::function<void(AxisMappingData*)>& action);

		void BindAction(IJGObject* _object, const String& actionName, EInputAction inputAction, const std::function<void()>& action, bool* pIsActive = nullptr);
		void UnBindAction(IJGObject* _object, const String& actionName);


		void BindAxis(IJGObject* _object, const String& axisName, const std::function<void(float)>& action, bool* pIsActive = nullptr);
		void UnBindAxis(IJGObject* _object, const String& actionName);
		void SetCenterPointWhenHideCursor(const JVector2& centerPos);
		void SetInputLock(bool isLock);
		bool IsInputLock() const;
	public:
		bool IsKeyPressed(EKeyCode code);
		bool IsKeyReleased(EKeyCode code);
		bool IsKeyDown(EKeyCode code);
		bool IsKeyUp(EKeyCode code);
		f32  GetMouseDeltaX() const;
		f32  GetMouseDeltaY() const;
		const JVector2& GetMouseDelta() const;
	private:
		void RefreshInput();
		void ActionMappingData_Update(EInputAction inputEvent, const String& mappingName);
		void AxisMappingData_Update(const String& mappingName, f32 value);
		EScheduleResult Update();


		bool IsAxisKeyCode(EKeyCode code) const;
		void SetAxisValue(EKeyCode code, f32 value);
		f32  GetAxisValue(EKeyCode code) const;
		f32  GetAxisDeadZone(EKeyCode code) const;
	};
}