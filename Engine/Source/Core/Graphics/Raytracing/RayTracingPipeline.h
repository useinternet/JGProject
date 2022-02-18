#pragma once
#include "JGCore.h"


namespace JG
{
	class IRootSignature;
	class IRayTracingPipeline
	{

	public:
		virtual ~IRayTracingPipeline() = default;

		virtual void AddLibrary(const String& shaderPath, const List<String>& symbolExports, bool isIncludeGlobalLib = true) = 0;
		virtual void AddLibraryAsSourceCode(const String& name, const String& sourceCode, const List<String>& symbolExports) = 0;
		virtual void AddHitGroup(const String& hitGroupName, const String& closestHitSymbol, const String& anyHitSymbol, const String& intersectionSymbol) = 0;
		virtual void SetMaxPayloadSize(u32 byteSize) = 0;
		virtual void SetMaxAttributeSize(u32 byteSize) = 0;
		virtual void SetMaxRecursionDepth(u32 maxDepth) = 0;
		virtual void SetGlobalRootSignature(SharedPtr<IRootSignature> rootSig) = 0;
		virtual void AddLocalRootSignature(SharedPtr<IRootSignature> rootSig, const List<String>& symbols) = 0;
		virtual bool IsValid() = 0;
		virtual bool Generate() = 0;
		virtual void Reset() = 0;

		virtual SharedPtr<IRootSignature> GetGlobalRootSignature() const = 0;
	public:
		static SharedPtr<IRayTracingPipeline> Create();
	};
}