#pragma once
#include "JGCore.h"


namespace JG
{
	class IRootSignature;
	class IRayTracingPipeline
	{

	public:
		virtual ~IRayTracingPipeline() = default;

		virtual void AddLibrary(const String& shaderPath, const List<String>& symbolExports) = 0;
		virtual void AddHitGroup(const String& hitGroupName, const String& closestHitSymbol, const String& anyHitSymbol, const String& intersectionSymbol) = 0;
		virtual void AddRayGenerationProgram(const String& entryPoint) = 0;
		virtual void AddHitProgram(const String& entryPoint) = 0;
		virtual void AddMissProgram(const String& entryPoint) = 0;
		virtual void SetMaxPayloadSize(u32 byteSize) = 0;
		virtual void SetMaxAttributeSize(u32 byteSize) = 0;
		virtual void SetMaxRecursionDepth(u32 maxDepth) = 0;
		virtual void SetGlobalRootSignature(SharedPtr<IRootSignature> rootSig) = 0;
		virtual bool IsValid() = 0;
		virtual u64 GetRayGenStartAddr() const = 0;
		virtual u64 GetRayGenSectionSize()const = 0;
		virtual u64 GetMissStartAddr() const = 0;
		virtual u64 GetMissSectionSize() const = 0;
		virtual u64 GetMissEntrySize() const = 0;
		virtual u64 GetHitGroupStartAddr() const = 0;
		virtual u64 GetHitGroupSectionSize() const = 0;
		virtual u64 GetHitGroupEntrySize() const = 0;
		virtual void Reset() = 0;
	public:

	};
}