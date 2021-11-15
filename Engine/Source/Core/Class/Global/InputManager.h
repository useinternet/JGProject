#pragma once
#include "JGCore.h"



namespace JG
{
	struct ActionMappingData
	{
		struct KeyData
		{
			EKeyCode Code;
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
			EKeyCode Code;
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

		KeyState mKeyState[256];
		List<SharedPtr<ActionMappingData>> mActionMappingsDataList;
		List<SharedPtr<AxisMappingData>>   mAxisMappingsDataList;

		Dictionary<String, ActionMappingData*>  mActionMappingsDataDic;
		Dictionary<String, AxisMappingData*>    mAxisMappingsDataDic;


	public:
		InputManager();
		virtual ~InputManager() = default;
	public:
		void AddActionMappings(const String& name);
		void AddAxisMappings(const String& name);
		void RemoveActionMappings(const String& name);
		void RemoveAxisMappings(const String& name);
		void ForEach(const std::function<void(ActionMappingData*)>& action);
		void ForEach(const std::function<void(AxisMappingData*)>& action);
	public:
		bool IsKeyPressed(EKeyCode code);
		bool IsKeyReleased(EKeyCode code);
		bool IsKeyDown(EKeyCode code);
		bool IsKeyUp(EKeyCode code);


	private:
		void Update();

	};
}