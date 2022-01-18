#pragma once




#include "JGCore.h"



namespace JG 
{
	class RenderStatistics
	{
	public:
		i32 TotalObjectCount = 0;
		i32 VisibleObjectCount = 0;
		i32 CullingObjectCount = 0;
	};


}