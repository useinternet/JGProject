#include "pch.h"
#include "DevPlayer.h"
#include "GameNode.h"
#include "Components/Camera.h"
#include "Components/Transform.h"

void DevPlayer::Start()
{
	JG::GameComponent::Start();

	//BindInputAxis("Foward", INPUT_AXIS_BIND(&DevPlayer::MoveForward));
	//BindInputAxis("RightLeft", INPUT_AXIS_BIND(&DevPlayer::MoveRight));
	//BindInputAxis("YawRotate", INPUT_AXIS_BIND(&DevPlayer::YawRotate));
	//BindInputAxis("PitchRotate", INPUT_AXIS_BIND(&DevPlayer::PitchRotate));
	//BindInputAction("DevTestAction", JG::EInputAction::Released, INPUT_ACTION_BIND(&DevPlayer::SetShowCursor));
}

void DevPlayer::Update()
{
	__super::Update();
	if (JG::InputManager::GetInstance().IsKeyPressed(JG::EKeyCode::A) == true)
	{
		JG_LOG_INFO("Dev Player Pressed A");
	}
}

void DevPlayer::MoveForward(JG::f32 value)
{
	if (IsShowCursor() == true) return;
	if (JG::Camera::GetMainCamera() == nullptr)
	{
		return;
	}
	auto transform = JG::Camera::GetMainCamera()->GetOwner()->GetTransform();
	auto location = transform->GetWorldLocation();
	auto lookVec = JG::Camera::GetMainCamera()->GetLook();
	lookVec.y = 0;
	lookVec = JG::JVector3::Normalize(lookVec);
	location += lookVec * value * GetTick() * 50;
	transform->SetWorldLocation(location);
}

void DevPlayer::MoveRight(JG::f32 value)
{
	if (IsShowCursor() == true) return;
	if (JG::Camera::GetMainCamera() == nullptr)
	{
		return;
	}
	auto transform = JG::Camera::GetMainCamera()->GetOwner()->GetTransform();
	auto location = transform->GetWorldLocation();
	auto rightVec = JG::Camera::GetMainCamera()->GetRight();
	rightVec.y = 0;
	rightVec = JG::JVector3::Normalize(rightVec);
	location += rightVec * value * GetTick() * 50;
	transform->SetWorldLocation(location);
}

void DevPlayer::YawRotate(JG::f32 value)
{
	if (IsShowCursor() == true) return;
	if (JG::Camera::GetMainCamera() == nullptr)
	{
		return;
	}
	auto transform = JG::Camera::GetMainCamera()->GetOwner()->GetTransform();
	auto rotation = transform->GetWorldRotation();


	rotation.y += value * GetTick() * 10;


	transform->SetWorldRotation(rotation);
}

void DevPlayer::PitchRotate(JG::f32 value)
{
	if (IsShowCursor() == true) return;
	if (JG::Camera::GetMainCamera() == nullptr)
	{
		return;
	}

	auto transform = JG::Camera::GetMainCamera()->GetOwner()->GetTransform();
	auto rotation = transform->GetWorldRotation();
	rotation.x += value * GetTick() * 10;
	transform->NormalizeRotation(rotation);


	if (rotation.x >= 0 && rotation.x < 180.0f)
	{
		rotation.x = JG::Math::Min(rotation.x, 70.0f);
	}
	else
	{
		rotation.x = JG::Math::Max(rotation.x, 290.0f);
	}



	transform->SetWorldRotation(rotation);
}


void DevPlayer::SetShowCursor()
{
	GameComponent::SetShowCursor(!IsShowCursor());
}
