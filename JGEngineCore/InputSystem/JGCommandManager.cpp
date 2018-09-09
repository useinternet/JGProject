#include"JGCommandManager.h"
#include"JGKeyCommand.h"
#include"JGAxisCommand.h"
#include"../EngineStatics/JMath/JGVector2D.h"
#include"JGPressManager.h"

using namespace std;
JGCommandManager::JGCommandManager()
{

}
JGCommandManager::~JGCommandManager()
{

}

void JGCommandManager::InitCommandManager(JGPressManager* Manager)
{
	m_pPressManager = Manager;
}
void JGCommandManager::Tick()
{
	for (auto& iter : m_mCommands)
	{
		iter.second->Tick();
	}
}
void JGCommandManager::RegisterKeyCommand(const EKey key, const std::wstring& CommandName)
{
	unique_ptr<JGCommand> Command = make_unique<JGKeyCommand>();
	Command->RegisterCommand(m_pPressManager, key, CommandName);
	m_mCommands.insert(pair<wstring, shared_ptr<JGCommand>>(CommandName, move(Command)));
}
void JGCommandManager::BindKeyCommand(const wstring& CommandName, const EKeyState KeyState, const function<void()>& Event)
{
	JGKeyCommand* Command = dynamic_cast<JGKeyCommand*>(m_mCommands[CommandName].get());
	if (Command)
	{
		Command->BindKeyCommand(KeyState, Event);
	}
}
void JGCommandManager::BindAxisCommand(const wstring& CommandName,const function<void(const float)>& Event)
{
	JGAxisCommand* Command = dynamic_cast<JGAxisCommand*>(m_mCommands[CommandName].get());
	if (Command)
	{
		Command->BindAxisCommand(Event);
	}
}

JGVector2D& JGCommandManager::GetMouseLocation()
{
	return m_pPressManager->GetMouseLocation();
}
