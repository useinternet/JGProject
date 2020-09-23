#pragma once
//#include "CommonCore.h"


#define ERROR_RANGE 0.00000005f
#define JG_PI 3.14159265359f

#define JG_2PI 6.28318530718f



class Math
{
public:
	inline constexpr static float ConvertToRadians(float fDegrees) { return fDegrees * (JG_PI / 180.0f); }
	inline constexpr static float ConvertToDegrees(float fRadians) { return fRadians * (180.0f / JG_PI); }
	/*
	  FXMVECTOR V,
  float     ViewportX,
  float     ViewportY,
  float     ViewportWidth,
  float     ViewportHeight,
  float     ViewportMinZ,
  float     ViewportMaxZ,
  FXMMATRIX Projection,
  CXMMATRIX View,
  CXMMATRIX World
	*/

};


