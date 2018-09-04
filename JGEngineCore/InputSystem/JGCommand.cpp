#include"JGCommand.h"
#include"JGPressManager.h"
using namespace std;
JGCommand::JGCommand()
{
	m_CommandName = make_unique<wstring>();
	*m_CommandName = TT("None");
}

JGCommand::~JGCommand()
{
}

void JGCommand::RegisterCommand(JGPressManager* PressManager, const EKey key, const wstring& CommandName)
{
	m_Key = key;
	*m_CommandName = CommandName;
	m_pPressManager = PressManager;
}
void JGCommand::Tick(const float DeltaTime)
{
	m_bIsPressed = m_pPressManager->IsPressed(m_Key);
}
EKey JGCommand::GetKey()
{
	return m_Key;
}
void JGCommand::SetKey(EKey key)
{
	m_Key = key;
}
bool JGCommand::GetIsPressed()
{
	return m_bIsPressed;
}
const wstring& JGCommand::GetCommandName()
{
	return *m_CommandName;
}

void JGCommand::SetCommandName(const wstring & CommandName)
{
	*m_CommandName = CommandName;
}
