#pragma once
#include"Common/JGRCCommon.h"
#include"Common/JGRCGlobalType.h"
#include"DirectXCommon.h"
namespace JGRC
{
	class CORE_EXPORT SpotLight
	{
	private:
		jgVec3 Position;
		real   RangeRcp;
		jgVec3 Direction;
		real   CosOuterCone;
		jgVec3 Color;
		real   CosInnerCone;
	public:
		SpotLight()
		{
			SetPosition(0.0f, 3.0f, 0.0f);
			SetRangeRcp(0.00001f);
			SetDirection(0.0f, -1.0f, 0.0f);
			SetColor(1.0f, 1.0f, 1.0f);
			SetOuterCone(65.0f);
			SetInnerConeRcp(55.0f);
		}
		void SetPosition(const real x, const real y, const real z)  { Position.set(x, y, z); }
		void SetRangeRcp(const real range)                          { RangeRcp = range; }
		void SetDirection(const real x, const real y, const real z) { Direction.set(x, y, z); }
		void SetColor(const real r, const real g, const real b)     { Color.set(r, g, b); }
		void SetOuterCone(const real Outercone)                     { CosOuterCone = COS(JGToRadian(Outercone));}
		void SetInnerConeRcp(const real InnerCone)                  { CosInnerCone = 1 / COS(JGToRadian(InnerCone)); }
	};
}