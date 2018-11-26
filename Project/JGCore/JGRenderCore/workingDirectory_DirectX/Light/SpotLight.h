#pragma once
#include"Light.h"
namespace JGRC
{
	class CORE_EXPORT SpotLight : public Light
	{
		// 0~2 Position
		// 3   RangeRcp
		// 4~6 Direction
		// 7   OuterCone
		// 8~10 Color
		// 11  InnerCone
	public:
		static uint const ParamCount = 12;
		jgVec3 m_LookAt;
	public:
		SpotLight()
		{
			Data.reserve(ParamCount);
			Data.resize(ParamCount);
			SetPosition(0.0f, 3.0f, 0.0f);
			SetRangeRcp(0.00001f);
			SetDirection(0.0f, -1.0f, 0.0f);
			SetColor(1.0f, 1.0f, 1.0f);
			SetOuterCone(65.0f);
			SetInnerConeRcp(55.0f);
		}
		virtual ~SpotLight() {}
		virtual real* GetData() override { return &Data[0]; }
		void SetPosition(const real x, const real y, const real z)  {
			bIsDynamic = true;
			Data[0] = x; Data[1] = y; Data[2] = z;
			}
		void SetRangeRcp(const real range)                          { 
			bIsDynamic = true;
			Data[3] = range;
			}
		void SetDirection(const real x, const real y, const real z) { 
			bIsDynamic = true;
			Data[4] = x; Data[5] = y; Data[6] = z;
		}
		void SetOuterCone(const real Outercone) {
			bIsDynamic = true;
			Data[7] = COS(JGToRadian(Outercone)); 
		}
		void SetColor(const real r, const real g, const real b)     { 
			bIsDynamic = true;
			Data[8] = r; Data[9] = g; Data[10] = b; 
		}
		void SetInnerConeRcp(const real InnerCone)                  { 
			bIsDynamic = true;
			Data[11] = 1 / COS(JGToRadian(InnerCone)); 
		}
		void SetLookAt(const real x, const real y, const real z)
		{
			m_LookAt.set(x, y, z);
		}
		virtual void CalcViewProjMatrix() override
		{
			ViewMatrix.lookAtLH(jgVec3(Data[0], Data[1], Data[2]), m_LookAt, jgVec3(0.0f, 1.0f, 0.0f));
			ProjMatrix.perspectiveFovLH(JGToRadian(Data[7]), GetShadowMapWidth() / GetShadowMapHeight(), GetNearZ(), GetFarZ());
			ViewProjMatrix = ViewMatrix * ProjMatrix;
			ViewProjMatrix.transpose();
		}
		const jgMatrix4x4& GetViewProjMatrix()
		{
			return ViewProjMatrix;
		}
	};
}