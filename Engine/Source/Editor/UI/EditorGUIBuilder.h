#pragma once
#include "JGCore.h"



namespace JG
{
#define DEFINE_GUI_COMMAND_CLASS(className, dataClass) \
	class className : public IGUICommand \
	{ \
		private: \
			GUICommandFunc Func; \
		public: \
			className##(const GUICommandFunc& func, const dataClass##& data = dataClass##()) : Func(func), Data(data) {} \
			virtual ~##className() = default; \
			virtual void Execute() override\
			{ \
				if (Func)\
				{\
					Func(this); \
				}\
			}\
			virtual Type GetCommandType() const override { return JGTYPE(className); } \
			static  Type GetStaticCommandType() { return JGTYPE(className); } \
			static className##* Cast(IGUICommand * arg) \
			{ \
				if (##className::GetStaticCommandType() == arg->GetCommandType()) \
				{ \
					return static_cast<##className*>(arg); \
				} \
				else return nullptr; \
			} \
			dataClass Data; \
	}; \
	

	class EditorGUIBuilder
	{
		class IGUICommand
		{
		public:
			virtual void  Execute() = 0;
			virtual Type  GetCommandType() const = 0;
			virtual ~IGUICommand() = default;
		};

		using GUICommandFunc = std::function<void(IGUICommand*)>;
		struct UnkownData
		{

		};
#pragma region GUICommandData
		struct GUICommandData_Label
		{
			String Label;

			GUICommandData_Label() = default;
			GUICommandData_Label(const String& label) : Label(label) {}
		};
#pragma endregion
#pragma region GUICommand
		DEFINE_GUI_COMMAND_CLASS(GUICommand_Label, GUICommandData_Label);
#pragma endregion
		// Begin
		// End
		u64 mID = 0;

		Queue<SharedPtr<IGUICommand>> mCommandQueue;
	
	public:
		void SetID(u64 id);
		void Label(const String& text);
		//
		void Execute();
	};
}