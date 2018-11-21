#pragma once
#include"Common/JGRCCommon.h"
#include"Common/JGRCGlobalType.h"
#include"DirectXCommon.h"



namespace JGRC
{
	class CORE_EXPORT DirectionLight
	{
	private:
		jgVec3 AmbientUp;
		real   padding1;
		jgVec3 AmbientDown;
		real   padding2;
		jgVec3 Direction;
		real   padding3;
		jgVec3 Color;
		real   padding4;
	public:
		DirectionLight()
		{
			SetAmbientDown(0.05f, 0.05f, 0.05f);
			SetAmbientUp(0.15f, 0.15f, 0.15f);
			SetColor(0.5f, 0.5f, 0.5f);
			SetDirection(0.0f, 0.0f, 1.0f);
		}
		void SetAmbientUp(const real r, const real g, const real b) { AmbientUp.set(r, g, b); }
		void SetAmbientDown(const real r, const real g, const real b) { AmbientDown.set(r, g, b); }
		void SetDirection(const real x, const real y, const real z) { Direction.set(x, y, z); }
		void SetColor(const real r, const real g, const real b)    { Color.set(r, g, b); }
	};


}