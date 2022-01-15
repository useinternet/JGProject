#pragma once
#include "Graphics/Raytracing/RayTracingPipeline.h"
#include "Platform/Graphics/DirectX12/Utill/RayTracingHelper.h"



namespace JG
{
	class IRootSignature;
	class RootSignature;
	class IComputeContext;
	class DirectX12RayTracingPipeline : public IRayTracingPipeline
	{
	private:
		Dictionary<String, ComPtr<IDxcBlob>> mShaderDic;
		SharedPtr<RootSignature>			 mRaytracingRootSig;
		ComPtr<ID3D12StateObject>			 mRaytracingPipelineState;
		ComPtr<ID3D12StateObjectProperties>  mRaytracingPipelineStateProperties;
		ComPtr<ID3D12Resource>				 mShaderBindingTableBuffer;

		RayTracingShaderBindingTableGenerator mSBTGen;
		RayTracingPipelineGenerator			  mPipelineGen;


		
		u64 mRayGenStartAddr   = 0;
		u64 mRayGenSectionSize = 0;

		u64 mMissStartAddr	 = 0;
		u64 mMissSectionSize = 0;
		u64 mMissEntrySize	 = 0;

		u64 mHitGroupStartAddr	 = 0;
		u64 mHitGroupSectionSize = 0;
		u64 mHitGroupEntrySize	 = 0;

	public:
		virtual ~DirectX12RayTracingPipeline();
		// Pipeline
		virtual void AddLibrary(const String& shaderPath, const List<String>& symbolExports) override;
		virtual void AddHitGroup(const String& hitGroupName, const String& closestHitSymbol, const String& anyHitSymbol, const String& intersectionSymbol)  override;
		virtual void AddRayGenerationProgram(const String& entryPoint) override;
		virtual void AddHitProgram(const String& entryPoint) override;
		virtual void AddMissProgram(const String& entryPoint) override;
		virtual void SetMaxPayloadSize(u32 byteSize) override;
		virtual void SetMaxAttributeSize(u32 byteSize) override;
		virtual void SetMaxRecursionDepth(u32 maxDepth) override;
		virtual void SetGlobalRootSignature(SharedPtr<IRootSignature> rootSig) override;
		virtual bool IsValid() override;
		virtual u64 GetRayGenStartAddr() const override;
		virtual u64 GetRayGenSectionSize()const override;
		virtual u64 GetMissStartAddr() const override;
		virtual u64 GetMissSectionSize() const override;
		virtual u64 GetMissEntrySize() const override;
		virtual u64 GetHitGroupStartAddr() const override;
		virtual u64 GetHitGroupSectionSize() const override;
		virtual u64 GetHitGroupEntrySize() const override;
		virtual void Reset() override;
	private:
		bool Generate();
		IDxcBlob* CompileShaderLibrary(const String& filePath);
	};


}