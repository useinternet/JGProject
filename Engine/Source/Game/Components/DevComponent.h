#pragma once
#include "GameComponent.h"

namespace JG
{
	class DevComponent : public GameComponent
	{
		JGCLASS
	private:
		bool mIsCursorShow = true;
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