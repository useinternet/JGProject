#pragma once
#include"Light.h"


namespace JGRC
{
	class CORE_EXPORT DirectionLight : public Light
	{
		// 0~2   AmbientUp
		// 4~6   AmbientDown
		// 8~10  Direction
		// 12~14 Color 
	public:
		static uint const ParamCount = 16;
	public:
		DirectionLight()
		{
			Data.reserve(ParamCount);
			Data.resize(ParamCount);
			SetAmbientDown(0.05f, 0.05f, 0.05f);
			SetAmbientUp(0.15f, 0.15f, 0.15f);
			SetColor(1.0f, 1.0f, 1.0f);
			SetDirection(0.0f, 0.0f, 1.0f);
			Data[3] = 0.0f; Data[7] = 0.0f; Data[11] = 0.0f; Data[15] = 0.0f;
		}
		virtual ~DirectionLight() {}
		virtual real* GetData() override { return &Data[0]; }
		void SetAmbientUp(const real r, const real g, const real b) {
			bIsDynamic = true;
			Data[0] = r; Data[1] = g; Data[2] = b;  }
		void SetAmbientDown(const real r, const real g, const real b)
		{
			bIsDynamic = true;
			Data[4] = r; Data[5] = g; Data[6] = b; }
		void SetDirection(const real x, const real y, const real z)
		{ 
			bIsDynamic = true;
			Data[8] = x; Data[9] = y; Data[10] = z; }
		void SetColor(const real r, const real g, const real b)    
		{
			bIsDynamic = true;
			Data[12] = r; Data[13] = g; Data[14] = b; }
		virtual void CalcViewProjMatrix() override
		{
			ViewMatrix.lookAtLH(jgVec3(0.0f,0.0f,0.0f), jgVec3(-Data[8], -Data[9], -Data[10]), jgVec3(0.0f, 1.0f, 0.0f));
			ProjMatrix.orthoLH(GetShadowMapWidth(), GetShadowMapHeight(), GetNearZ(), GetFarZ());
			ViewProjMatrix = ViewMatrix * ProjMatrix;
			ViewProjMatrix.transpose();
		}
		const jgMatrix4x4& GetViewProjMatrix()
		{
			return ViewProjMatrix;
		}
	private:
	};
}