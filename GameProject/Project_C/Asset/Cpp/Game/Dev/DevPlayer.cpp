#include "pch.h"
#include "DevPlayer.h"



void DevPlayer::Start()
{
	JG_LOG_INFO("Start Dev Player");
}

void DevPlayer::Update()
{
	if (JG::InputManager::GetInstance().IsKeyPressed(JG::EKeyCode::A) == true)
	{
		JG_LOG_INFO("Dev Player Pressed A");
	}
}
