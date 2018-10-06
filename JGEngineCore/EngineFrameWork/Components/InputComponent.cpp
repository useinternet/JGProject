#include"Component.h"
#include "InputComponent.h"
using namespace std;
InputComponent::InputComponent()
{
	RegisterComponentID(typeid(this));
}

InputComponent::~InputComponent()
{
	for (auto& iter : m_mCommand)
	{
		if (iter.second == Key)
		{
			GetCommandManager()->KillKeyCommand(iter.first);
		}
		else
		{
			GetCommandManager()->KillAxisCommand(iter.first);
		}
	}
}
void InputComponent::BindKeyCommand(const wstring& CommandName,const EKeyState KeyState, const function<void()>& Event)
{
	m_mCommand.insert(pair<const wstring, ECommandType>(CommandName, Key));
	GetCommandManager()->BindKeyCommand(CommandName, KeyState, Event);
}
void InputComponent::BindAxisCommand(const std::wstring& CommandName, const std::function<void(const float)>& Event)
{
	m_mCommand.insert(pair<const wstring, ECommandType>(CommandName, Axis));
	GetCommandManager()->BindAxisCommand(CommandName, Event);
}

