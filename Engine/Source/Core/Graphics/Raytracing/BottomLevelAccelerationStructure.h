#pragma once


#include "JGCore.h"
namespace JG
{
	class IVertexBuffer;
	class IIndexBuffer;
	class IComputeContext;
	class IBottomLevelAccelerationStructure
	{

	public:
		virtual ~IBottomLevelAccelerationStructure() = default;

		virtual void Generate(
			SharedPtr<IComputeContext> context, SharedPtr<IVertexBuffer> vBuffer, SharedPtr<IIndexBuffer> iBuffer, bool onlyUpdate = false, bool allowUpdate = false, bool isOpaque = true) = 0;
		virtual void Reset() = 0;
	public:
		static SharedPtr<IBottomLevelAccelerationStructure> Create();
	};
}
