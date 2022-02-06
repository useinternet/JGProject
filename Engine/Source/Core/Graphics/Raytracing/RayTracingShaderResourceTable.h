#pragma once
#include "JGCore.h"
#include "LocalRootArgument.h"
namespace JG
{
	class IRayTracingShaderResourceTable
	{
	public:
		~IRayTracingShaderResourceTable() = default;

	public:
		virtual void AddRayGeneration(const String& entryPoint) = 0;
		virtual void AddMiss(const String& entryPoint) = 0;
		virtual void AddHitGroupAndBindLocalRootArgument(const String& entryPoint, const LocalRootArgument& localRootArg = LocalRootArgument()) = 0;
		virtual void Reset() = 0;

	public:
		static SharedPtr<IRayTracingShaderResourceTable> Create();
	};
}