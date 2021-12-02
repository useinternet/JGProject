#include "pch.h"
#include "DevComponent.h"
#include "GameNode.h"
#include "Camera.h"
#include "Transform.h"

namespace JG
{

	void DevComponent::Start()
	{
		__super::Start();
		BindInputAxis("Foward",  INPUT_AXIS_BIND(&DevComponent::MoveForward));
		BindInputAxis("RightLeft", INPUT_AXIS_BIND(&DevComponent::MoveRight));
		BindInputAxis("YawRotate", INPUT_AXIS_BIND(&DevComponent::YawRotate));
		BindInputAxis("PitchRotate", INPUT_AXIS_BIND(&DevComponent::PitchRotate));
		BindInputAction("DevTestAction", EInputAction::Released, INPUT_ACTION_BIND(&DevComponent::SetShowCursor));

	}
	void DevComponent::Update()
	{
		__super::Update();

	}

	void DevComponent::MoveForward(f32 value)
	{
		if (IsShowCursor() == true) return;
		if (Camera::GetMainCamera() == nullptr)
		{
			return;
		}
		auto transform = Camera::GetMainCamera()->GetOwner()->GetTransform();
		auto location  = transform->GetWorldLocation();
		auto lookVec   = Camera::GetMainCamera()->GetLook();
		lookVec.y = 0;
		lookVec = JVector3::Normalize(lookVec);
		location += lookVec * value * GetTick() * 50;
		transform->SetWorldLocation(location);
	}

	void DevComponent::MoveRight(f32 value)
	{
		if (IsShowCursor() == true) return;
		if (Camera::GetMainCamera() == nullptr)
		{
			return;
		}
		auto transform = Camera::GetMainCamera()->GetOwner()->GetTransform();
		auto location = transform->GetWorldLocation();
		auto rightVec = Camera::GetMainCamera()->GetRight();
		rightVec.y = 0;
		rightVec = JVector3::Normalize(rightVec);
		location += rightVec * value * GetTick() * 50;
		transform->SetWorldLocation(location);
		
	}

	void DevComponent::YawRotate(f32 value)
	{
		if (IsShowCursor() == true) return;
		if (Camera::GetMainCamera() == nullptr)
		{
			return;
		}
		auto transform = Camera::GetMainCamera()->GetOwner()->GetTransform();
		auto rotation = transform->GetWorldRotation();
		

		rotation.y += value * GetTick() * 10;


		transform->SetWorldRotation(rotation);
	}

	void DevComponent::PitchRotate(f32 value)
	{
		if (IsShowCursor() == true) return;
		if (Camera::GetMainCamera() == nullptr)
		{
			return;
		}

		auto transform = Camera::GetMainCamera()->GetOwner()->GetTransform();
		auto rotation = transform->GetWorldRotation();
		rotation.x += value * GetTick() * 10;
		transform->NormalizeRotation(rotation);


		if (rotation.x >= 0 && rotation.x < 180.0f)
		{
			rotation.x = Math::Min(rotation.x, 70.0f);
		}
		else
		{
			rotation.x = Math::Max(rotation.x, 290.0f);
		}



		transform->SetWorldRotation(rotation);
	}


	void DevComponent::SetShowCursor()
	{
		GameComponent::SetShowCursor(!IsShowCursor());
	}


}