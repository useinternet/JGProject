#include "pch.h"
#include "DevPlayer.h"
#include "Class/Asset/Asset.h"
#include "GameNode.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/SkeletalMeshRenderer.h"
#include "Animation/AnimationController.h"
#include "Animation/AnimationParameters.h"

void DevPlayer::Start()
{
	JG::GameComponent::Start();

	BindInputAxis("Foward", INPUT_AXIS_BIND(&DevPlayer::MoveForward));
	BindInputAxis("RightLeft", INPUT_AXIS_BIND(&DevPlayer::MoveRight));
	//BindInputAxis("YawRotate", INPUT_AXIS_BIND(&DevPlayer::YawRotate));
	//BindInputAxis("PitchRotate", INPUT_AXIS_BIND(&DevPlayer::PitchRotate));
	BindInputAction("DevTestAction", JG::EInputAction::Released, INPUT_ACTION_BIND(&DevPlayer::SetShowCursor));

	mCamera = GetOwner()->FindNode(0)->FindComponent<JG::Camera>();
	mRenderer = GetOwner()->FindComponent<JG::SkeletalMeshRenderer>();

	if (mRenderer != nullptr)
	{
		JG::Asset<JG::AnimationController>* animAsset = mRenderer->GetAnimation();
		if (animAsset != nullptr && animAsset->IsValid())
		{
			mAnimController = animAsset->Get().get();
			mAnimParams = animAsset->Get()->GetAnimationParameters().get();
		}
	}

}

void DevPlayer::Update()
{
	__super::Update();
	if (JG::InputManager::GetInstance().IsKeyPressed(JG::EKeyCode::A) == true)
	{
		JG_LOG_INFO("Dev Player Pressed A");
	}
	if (mAnimParams)
	{
		if (mIsRunning == true)
		{
			if (JG::InputManager::GetInstance().IsKeyDown(JG::EKeyCode::LeftShift) == true)
			{
				mAnimParams->SetFloat("Velocity", 100.0f);
				JG_LOG_INFO("Velocity 100");
			}
			else
			{
				mAnimParams->SetFloat("Velocity", 10.0f);
				JG_LOG_INFO("Velocity 10");
			}
		}
		else
		{
			mAnimParams->SetFloat("Velocity", 0.0f);
			JG_LOG_INFO("Velocity 0");
		}
		mIsRunning = false;
	}

}

void DevPlayer::MoveForward(JG::f32 value)
{
	//if (IsShowCursor() == true) return;
	if (mCamera == nullptr)
	{
		return;
	}
	JG::Transform* transform = GetOwner()->GetTransform();
	JG::JVector3 location = transform->GetWorldLocation();

	JG::JVector3 camLook  = mCamera->GetLook();
	camLook.y = 0;
	camLook = JG::JVector3::Normalize(camLook);

	// 위치 변경
	location += camLook * value;
	transform->SetWorldLocation(location);

	mIsRunning = true;
	//JG::JVector3 objLook = transform->GetLook();
	//objLook.y = 0;
	//objLook = JG::JVector3::Normalize(objLook);

	//JG::JQuaternion q = JG::JQuaternion::FromTwoVectors(camLook, objLook);
	//transform->SetWorldQuaternion(q);
}

void DevPlayer::MoveRight(JG::f32 value)
{
	//if (IsShowCursor() == true) return;
	if (mCamera == nullptr)
	{
		return;
	}
	JG::Transform* transform = GetOwner()->GetTransform();
	JG::JVector3 location = transform->GetWorldLocation();
	JG::JVector3 camRight = mCamera->GetRight();
	camRight.y = 0;
	camRight = JG::JVector3::Normalize(camRight);
	location += camRight * value;
	transform->SetWorldLocation(location);


	mIsRunning = true;
	//JG::JVector3 objRight = transform->GetRight();
	//objRight.y = 0;
	//objRight = JG::JVector3::Normalize(objRight);

	//JG::JQuaternion q = JG::JQuaternion::FromTwoVectors(camRight, objRight);
	//transform->SetWorldQuaternion(q);
}

void DevPlayer::YawRotate(JG::f32 value)
{
	//if (IsShowCursor() == true) return;
	if (JG::Camera::GetMainCamera() == nullptr)
	{
		return;
	}
	JG::Transform* camTransform = JG::Camera::GetMainCamera()->GetOwner()->GetTransform();
	JG::Transform* playerTransform = GetOwner()->GetTransform();
	JG::JVector3 rotation = camTransform->GetWorldRotation();

	// 카메라가 지금
	// 플레이어의 Up Vector와 카메라
	JG::JVector3 playerUpVec = playerTransform->GetUp();
	JG::JVector3 dir = JG::JVector3::Normalize(camTransform->GetWorldLocation() - playerTransform->GetWorldLocation());
	//
	// Y
	GetOwner()->GetTransform();
	rotation.y += value * GetTick() * 10;


	camTransform->SetWorldRotation(rotation);
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
