#pragma once
#include "GameBridge.h"


namespace JG
{
	class Camera;
	class SkeletalMeshRenderer;
	class AnimationController;
	class AnimationParameters;
}


REGISTER_GAMECOMPONENT(DevPlayer)
class GAME_API DevPlayer : public JG::GameComponent
{
	JG_GAMECOMPONENT
private:
	JG::f32 mCamLength = 1000.0f;
	JG::Camera* mCamera = nullptr;
	JG::SkeletalMeshRenderer* mRenderer = nullptr;
	JG::AnimationController* mAnimController = nullptr;
	JG::AnimationParameters* mAnimParams = nullptr;

	bool mIsRunning = false;
protected:
	virtual void Start()  override;
	virtual void Update() override;

	void MoveForward(JG::f32 value);
	void MoveRight(JG::f32 value);
	void YawRotate(JG::f32 value);
	void PitchRotate(JG::f32 value);
	void SetShowCursor();
};