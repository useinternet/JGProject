#pragma once
#include"Common/JGRCCommon.h"

namespace JGRC
{
	class CORE_EXPORT Light
	{
		friend class SRSShadowPass;
	private:
		static real ShadowWidth;
		static real ShadowHeight;
		static real NearZ;
		static real FarZ;
	protected:
		std::vector<real> Data;
		jgMatrix4x4 ViewMatrix;
		jgMatrix4x4 ProjMatrix;
		jgMatrix4x4 ViewProjMatrix;
		bool bIsDynamic = false;
	public:
		Light() {}
		virtual ~Light() {}
		virtual real* GetData() = 0;
		virtual bool IsDynamic() { return bIsDynamic; }
		virtual void Static() { bIsDynamic = false; }
		virtual void Dynamic() { bIsDynamic = true; }
		virtual void CalcViewProjMatrix() = 0;
	protected:
		real GetShadowMapWidth()  { return ShadowWidth;  }
		real GetShadowMapHeight() { return ShadowHeight; }
		real GetNearZ() { return NearZ; }
		real GetFarZ()  { return FarZ; }
	};
}