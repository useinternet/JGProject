#pragma once
#include "Graphics/Raytracing/RayTracingPipeline.h"
#include "Platform/Graphics/DirectX12/Utill/RayTracingHelper.h"



namespace JG
{

	class RootSignature;
	class IComputeContext;
	class DirectX12RayTracingPipeline : public IRayTracingPipeline
	{
	private:
		Dictionary<String, ComPtr<IDxcBlob>> mShaderDic;
		SharedPtr<IRootSignature>			 mRaytracingRootSig;
		ComPtr<ID3D12StateObject>			 mRaytracingPipelineState;
		ComPtr<ID3D12StateObjectProperties>  mRaytracingPipelineStateProperties;
		RayTracingPipelineGenerator			  mPipelineGen;

		bool mIsDirty = true;
	public:
		virtual ~DirectX12RayTracingPipeline();
		// Pipeline
		virtual void AddLibrary(const String& shaderPath, const List<String>& symbolExports, bool isIncludeGlobalLib = true) override;
		virtual void AddLibraryAsSourceCode(const String& name, const String& sourceCode, const List<String>& symbolExports) override;
		virtual void AddHitGroup(const String& hitGroupName, const String& closestHitSymbol, const String& anyHitSymbol, const String& intersectionSymbol)  override;
		virtual void SetMaxPayloadSize(u32 byteSize) override;
		virtual void SetMaxAttributeSize(u32 byteSize) override;
		virtual void SetMaxRecursionDepth(u32 maxDepth) override;
		virtual void SetGlobalRootSignature(SharedPtr<IRootSignature> rootSig) override;
		virtual void AddLocalRootSignature(SharedPtr<IRootSignature> rootSig, const List<String>& symbols) override;
		virtual bool IsValid() override;
		virtual bool Generate() override;
		virtual void Reset() override;

		virtual SharedPtr<IRootSignature> GetGlobalRootSignature() const override;
	public:
		ID3D12StateObjectProperties* GetPipelineProperties() const {
			return mRaytracingPipelineStateProperties.Get();
		}
		ID3D12StateObject* GetPipelineState() const {
			return mRaytracingPipelineState.Get();
		}
	private:
		IDxcBlob* CompileShaderLibrary(const String& filePath, const String& sourceCode);
	};
}