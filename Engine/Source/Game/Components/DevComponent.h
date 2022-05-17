#pragma once
#include "GameComponent.h"

namespace JG
{
	class DevComponent : public GameComponent
	{
		JGCLASS

		f32 mCamLength = 1000.0f;
		f32 mSpeed = 1.5f;
		Camera* mCamera = nullptr;
		class SkeletalMeshRenderer* mRenderer = nullptr;
		class AnimationController* mAnimController = nullptr;
		class AnimationParameters* mAnimParams = nullptr;
		bool mIsRunning = false;
	protected:
		virtual void Start()  override;
		virtual void Update() override;
	private:
		void MoveForward(f32 value);
		void MoveRight(f32 value);
		void YawRotate(f32 value);
		void PitchRotate(f32 value);
		void SetShowCursor();
	};
}