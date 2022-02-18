#pragma once
#include "JGCore.h"


namespace JG
{
	class IBottomLevelAccelerationStructure;
	class IComputeContext;
	class IStructuredBuffer;
	class ITopLevelAccelerationStructure
	{


	public:
		virtual ~ITopLevelAccelerationStructure() = default;
		virtual SharedPtr<IStructuredBuffer> GetPrevFrameTransformBuffer() const = 0;
		virtual void AddInstance(SharedPtr<IBottomLevelAccelerationStructure> btAS, const JMatrix& transform, u32 instanceID, u32 hitGroupIndex) = 0;
		virtual void Generate(SharedPtr<IComputeContext> context, bool onlyUpdate = false, bool allowUpdate = false) = 0;
		virtual void Reset() = 0;

	public:
		static SharedPtr<ITopLevelAccelerationStructure> Create();
	};




}