#include"Component.h"
#include "InputComponent.h"
using namespace std;
InputComponent::InputComponent()
{
	RegisterComponentID(typeid(this));

}

InputComponent::~InputComponent()
{


}
void InputComponent::BindKeyCommand(const wstring& CommandName,const EKeyState KeyState, const function<void()>& Event)
{
	GetCommandManager()->BindKeyCommand(CommandName, KeyState, Event);
}
void InputComponent::BindAxisCommand(const std::wstring& CommandName, const std::function<void(const float)>& Event)
{
	GetCommandManager()->BindAxisCommand(CommandName, Event);
}
