#include "pch.h"
#include "CommandPrompt.h"


#define COMMAND_BIND_FUNC(x) std::bind(&CommandPrompt::##x, this, std::placeholders::_1, std::placeholders::_2)
namespace JG
{
	CommandPrompt::CommandPrompt()
	{
		LoadCommand();
	}
	void CommandPrompt::Execute(const String& command)
	{
		JG_LOG_TRACE(">> {0}", command);

		List<String> commandAndArgs = StringHelper::Split(command, ' ');
		if (commandAndArgs.empty())
		{
			return;
		}
		String commend = commandAndArgs[0];
		
		if (mRegisteredCommand.find(command) == mRegisteredCommand.end())
		{
			JG_LOG_ERROR(">> Does Not Exist Command ..");
			return;
		}


		List<String> args;
		if (commandAndArgs.size() > 1)
		{
			args.insert(args.end(), commandAndArgs.begin() + 1, commandAndArgs.end());
		}
		Execute(command, args);
	}
	void CommandPrompt::Execute(const String& command, const List<String>& args)
	{
		mRegisteredCommand[command](command, args);
	}
	void CommandPrompt::LoadCommand()
	{
		RegisterCommand("TestCommand", COMMAND_BIND_FUNC(TestCommand));


		

	}
	void CommandPrompt::RegisterCommand(const String& command, const std::function<void(const String&, const List<String>&)>& func)
	{
		mRegisteredCommand[command] = func;
	}
	void CommandPrompt::TestCommand(const String& command, const List<String>& args)
	{
		JG_LOG_ERROR(">> Test Command!! ..");
	}
}