#pragma once
#include "GameBridge.h"


REGISTER_GAMECOMPONENT(DevPlayer)
class GAME_API DevPlayer : public JG::GameComponent
{
	JG_GAMECOMPONENT
protected:
	virtual void Start()  override;
	virtual void Update() override;


};