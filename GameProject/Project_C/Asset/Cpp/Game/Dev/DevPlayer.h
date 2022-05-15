#pragma once
#include "GameBridge.h"


namespace JG
{
	class Camera;
}


REGISTER_GAMECOMPONENT(DevPlayer)
class GAME_API DevPlayer : public JG::GameComponent
{
	JG_GAMECOMPONENT
protected:
	virtual void Start()  override;
	virtual void Update() override;

	void MoveForward(JG::f32 value);
	void MoveRight(JG::f32 value);
	void YawRotate(JG::f32 value);
	void PitchRotate(JG::f32 value);
	void SetShowCursor();
};