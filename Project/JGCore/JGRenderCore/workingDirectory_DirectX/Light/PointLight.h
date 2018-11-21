#pragma once
#include"Common/JGRCCommon.h"
#include"Common/JGRCGlobalType.h"
#include"DirectXCommon.h"


namespace JGRC
{
	class CORE_EXPORT PointLight
	{
	private:
		jgVec3 Position;
		real   RangeRcp;
		jgVec3 Color;
		real   padding;
	public:
		PointLight()
		{
			SetPosition(0.0f, 0.0f, -3.0f);
			SetRangeRcp(0.01f);
			SetColor(1.0f, 1.0f, 1.0f);
		}
		void SetPosition(const real x, const real y, const real z) { Position.set(x, y, z); }
		void SetRangeRcp(const real range) { RangeRcp = range; }
		void SetColor(const real r, const real g, const real b) { Color.set(r, g, b); }
	};
}