#include "pch.h"
#include "DevComponent.h"
#include "Class/Asset/Asset.h"
#include "GameNode.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/SkeletalMeshRenderer.h"
#include "Animation/AnimationController.h"
#include "Animation/AnimationParameters.h"


namespace JG
{
	void DevComponent::Start()
	{
		JG::GameComponent::Start();

		BindInputAxis("Foward", INPUT_AXIS_BIND(&DevComponent::MoveForward));
		BindInputAxis("RightLeft", INPUT_AXIS_BIND(&DevComponent::MoveRight));
		BindInputAxis("YawRotate", INPUT_AXIS_BIND(&DevComponent::YawRotate));
		BindInputAxis("PitchRotate", INPUT_AXIS_BIND(&DevComponent::PitchRotate));
		BindInputAction("DevTestAction", JG::EInputAction::Released, INPUT_ACTION_BIND(&DevComponent::SetShowCursor));


		mRenderer = GetOwner()->FindNode(0)->FindComponent<JG::SkeletalMeshRenderer>();
		mCamera = GetOwner()->FindNode(1)->FindComponent<JG::Camera>();
		if (mRenderer != nullptr)
		{
			JG::Asset<JG::AnimationController>* animAsset = mRenderer->GetAnimation();
			if (animAsset != nullptr && animAsset->IsValid())
			{
				mAnimController = animAsset->Get().get();
				mAnimParams = animAsset->Get()->GetAnimationParameters().get();
			}
		}
		JG::Transform* camTransform    = mCamera->GetOwner()->GetTransform();
		JG::Transform* playerTransform = GetOwner()->GetTransform();

		mCamLength = JVector3::Length(playerTransform->GetWorldLocation() - camTransform->GetWorldLocation());
	}

	void DevComponent::Update()
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

	void DevComponent::MoveForward(JG::f32 value)
	{
		if (IsShowCursor() == true) return;
		if (mCamera == nullptr)
		{
			return;
		}
		JG::JVector3 location = GetOwner()->GetTransform()->GetWorldLocation();

		JG::JVector3 camLook = mCamera->GetLook();
		camLook.y = 0;
		camLook = JG::JVector3::Normalize(camLook * value);

		// 위치 변경
		location += camLook * mSpeed;
		GetOwner()->GetTransform()->SetWorldLocation(location);

		mIsRunning = true;
		JG::JVector3 objLook = mRenderer->GetOwner()->GetTransform()->GetLook();
		objLook.y = 0;
		objLook = JG::JVector3::Normalize(objLook * value);

		JG::JQuaternion q = JG::JQuaternion::FromTwoVectors(camLook, objLook);
		mRenderer->GetOwner()->GetTransform()->SetLocalQuaternion(q);
	}

	void DevComponent::MoveRight(JG::f32 value)
	{
		if (IsShowCursor() == true) return;
		if (mCamera == nullptr)
		{
			return;
		}
		JG::Transform* transform = GetOwner()->GetTransform();
		JG::JVector3 location = transform->GetWorldLocation();
		JG::JVector3 camRight = mCamera->GetRight();
		camRight.y = 0;
		camRight = JG::JVector3::Normalize(camRight * value);
		location += camRight * mSpeed;
		transform->SetWorldLocation(location);


		mIsRunning = true;
		JG::JVector3 objLook = mRenderer->GetOwner()->GetTransform()->GetLook();
		objLook.y = 0;
		objLook = JG::JVector3::Normalize(objLook * value);

		JG::JQuaternion q = JG::JQuaternion::FromTwoVectors(objLook, camRight);
		mRenderer->GetOwner()->GetTransform()->SetLocalQuaternion(q);
	}

	void DevComponent::YawRotate(JG::f32 value)
	{
		if (IsShowCursor() == true) return;
		if (JG::Camera::GetMainCamera() == nullptr)
		{
			return;
		}
		JG::Transform* camTransform = mCamera->GetOwner()->GetTransform();
		JG::Transform* playerTransform = GetOwner()->GetTransform();
		JVector3 targetPos = playerTransform->GetWorldLocation();


		JVector3 rotation = camTransform->GetWorldRotation();


		GetOwner()->GetTransform();
		rotation.y += value * GetTick() * 10;
	

		camTransform->SetLocalRotation(rotation);
		camTransform->SetWorldLocation(targetPos - camTransform->GetLook() * mCamLength);
	}

	void DevComponent::PitchRotate(JG::f32 value)
	{
		if (IsShowCursor() == true) return;

		JG::Transform* playerTransform = GetOwner()->GetTransform();
		JVector3 targetPos = playerTransform->GetWorldLocation();


		auto transform = mCamera->GetOwner()->GetTransform();
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



		transform->SetLocalRotation(rotation);
		transform->SetWorldLocation(targetPos - transform->GetLook() * mCamLength);
	}


	void DevComponent::SetShowCursor()
	{
		GameComponent::SetShowCursor(!IsShowCursor());
	}
}