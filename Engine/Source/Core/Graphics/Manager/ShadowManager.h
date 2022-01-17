#pragma once
#include "JGCore.h"
#include "Graphics/GraphicsDefine.h"










namespace JG
{
	class ITopLevelAccelerationStructure;
	class IRayTracingPipeline;
	class IBottomLevelAccelerationStructure;
	class ShadowManager
	{

		bool mIsRayTracedShadow = false;
	public:
		void Init(bool isRayTraced);



	};
}