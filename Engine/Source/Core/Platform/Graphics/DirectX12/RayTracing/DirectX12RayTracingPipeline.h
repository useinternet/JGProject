#pragma once
#include "JGCore.h"
#include "Platform/Graphics/DirectX12/Utill/RayTracingHelper.h"



namespace JG
{
	class RootSignature;
	class DirectX12RayTracingPipeline
	{
	private:
		//Dictionary<String, ShaderParam>		 mShaderParamDic;
		Dictionary<String, ComPtr<IDxcBlob>> mShaderDic;


		i32 mRootParam = 0;


		SharedPtr<RootSignature>			 mRaytracingRootSig;
		ComPtr<ID3D12StateObject>			 mRaytracingPipelineState;
		ComPtr<ID3D12StateObjectProperties>  mRaytracingPipelineStateProperties;
		ComPtr<ID3D12Resource>				 mShaderBindingTableBuffer;

		RayTracingShaderBindingTableGenerator mSBTGen;
		RayTracingPipelineGenerator			  mPipelineGen;
	public:
		DirectX12RayTracingPipeline();
	public:
		// Pipeline
		void AddLibrary(const String& shaderPath, const List<String>& symbolExports);
		void AddHitGroup(const String& hitGroupName, const String& closestHitSymbol, const String& anyHitSymbol, const String& intersectionSymbol);


		void AddRayGenerationProgram(const String& entryPoint);
		void AddHitProgram(const String& entryPoint);
		void AddMissProgram(const String& entryPoint);
		void SetMaxPayloadSize(u32 byteSize);
		void SetMaxAttributeSize(u32 byteSize);
		void SetMaxRecursionDepth(u32 maxDepth);

		bool Finalize();
		bool IsValid();
	private:
		IDxcBlob* CompileShaderLibrary(const String& filePath);
	};


}