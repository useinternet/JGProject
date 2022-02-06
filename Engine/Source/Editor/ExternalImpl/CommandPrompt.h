#pragma once


#include "JGCore.h"

namespace JG
{
	class CommandPrompt
	{
		Dictionary<String, std::function<void(const String&, const List<String>&)>> mRegisteredCommand;
	public:
		CommandPrompt();
	public:
		void Execute(const String& command);
	private:
		void Execute(const String& command, const List<String>& args);
		void LoadCommand();
		void RegisterCommand(const String& command, const std::function<void(const String&, const List<String>&)>& func);
	private:
		void TestCommand(const String& command, const List<String>& args);
	};
}