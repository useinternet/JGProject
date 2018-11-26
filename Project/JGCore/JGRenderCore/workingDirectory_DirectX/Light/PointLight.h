#pragma once
#include"Light.h"


namespace JGRC
{
	class CORE_EXPORT PointLight : public Light
	{
		// 0~2 Position
		// 3   Range
		// 4~6 Color
	public:
		static uint const ParamCount = 8;
	public:
		PointLight()
		{
			Data.reserve(ParamCount);
			Data.resize(ParamCount);
			SetPosition(0.0f, 0.0f, -3.0f);
			SetRangeRcp(0.01f);
			SetColor(1.0f, 1.0f, 1.0f);
			Data[7] = 0.0f;
		}
		virtual ~PointLight() {}
		virtual real* GetData() override { return &Data[0]; }
		void SetPosition(const real x, const real y, const real z){
			bIsDynamic = true;
			Data[0] = x; Data[1] = y; Data[2] = z;
		}
		void SetRangeRcp(const real range) { 
			bIsDynamic = true;
			Data[3] = range;
		}
		void SetColor(const real r, const real g, const real b) {
			bIsDynamic = true;
			Data[4] = r; Data[5] = g; Data[6] = b;
		 }
		virtual void CalcViewProjMatrix() override
		{

		}
	};
}