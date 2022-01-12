#pragma once
#include "DirectX12Helper.h"




namespace JG
{
	class CommandList;
	class RayTracingPipelineGenerator
	{
		class Library
		{
		public:
			Library(IDxcBlob* dxil, const List<String>& exportedSymbols);
			Library(const Library& source);

			IDxcBlob* Dxil;
			List<std::wstring> ExportedSymbols;

			List<D3D12_EXPORT_DESC> Exports;
			D3D12_DXIL_LIBRARY_DESC LibDesc;
		};
		struct HitGroup
		{
			HitGroup(const String& hitGroupName, const String& closestHitSymbol,
				const String& anyHitSymbol = "", const String& intersectionSymbol = "");
			HitGroup(const std::wstring& hitGroupName, const std::wstring& closestHitSymbol,
				const std::wstring& anyHitSymbol, const std::wstring& intersectionSymbol);
			HitGroup(const HitGroup& source);

			std::wstring HitGroupName;
			std::wstring ClosestHitSymbol;
			std::wstring AnyHitSymbol;
			std::wstring IntersectionSymbol;
			D3D12_HIT_GROUP_DESC Desc = {};
		};

		List<Library>  mLibraries = {};
		List<HitGroup> mHitGroups = {};
		ComPtr<ID3D12RootSignature> mGlobalRootSignature;


		u32 mMaxPayLoadSize = 0;
		u32 mMaxAttributeSize = 2 * sizeof(float);
		u32 mMaxRecursionDepth = 1;


	public:
		void AddLibrary(IDxcBlob* dxilLib, const List<String>& symbolExports);
		void AddHitGroup(const String& hitGroupName, const String& closestHitSymbol, const String& anyHitSymbol = "", const String& intersectionSymbol = "");
		void SetRootSignature(ID3D12RootSignature* rootSig);
		void SetMaxPayloadSize(u32 btSize);
		void SetMaxAttributeSize(u32 btSize);
		void SetMaxRecursionDepth(u32 maxDepth);
		ComPtr<ID3D12StateObject> Generate();
	};

	class BottomLevelASGenerator
	{
	private:
		List<D3D12_RAYTRACING_GEOMETRY_DESC> mVertexBuffers = {};
		u64 mScratchSize = 0;
		u64 mResultSize = 0;
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS mFlags;
	public:
		void AddVertexBuffer(
			ID3D12Resource* vertexBuffer, u64 vertexOffset,  u32 vertexCount, u32 vertexSize,     
			ID3D12Resource* transformBuffer, u64 transformOffset,bool isOpaque = true );

		void AddVertexBuffer(
			ID3D12Resource* vertexBuffer, u64 vertexOffset, u32 vertexCount, u32 vertexSize,
			ID3D12Resource* indexBuffer, u64 indexOffset, u32 indexCount,
			ID3D12Resource* transformBuffer, u64 transformOffset,   bool isOpaque = true);

	
		void ComputeASBufferSizes(bool allowUpdate,  UINT64* scratchSize, UINT64* resultSize);

		void Generate(
			CommandList* commandList, ID3D12Resource* scratchBuffer, ID3D12Resource* resultBuffer,
			bool updateOnly = false,  ID3D12Resource* previousResult = nullptr);
	};

	class TopLevelASGenerator
	{
	private:
		struct Instance
		{
			Instance(ID3D12Resource* blAS, const JMatrix& tr, u32 iID, u32 hgId) :
				BottomLevelAS(blAS), Transform(tr), InstanceID(iID), HitGroupIndex(hgId) {}

			ID3D12Resource* BottomLevelAS;
			const JMatrix& Transform;
			u32 InstanceID;
			u32 HitGroupIndex;
		};
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS mFlags;
		List<Instance> mInstances;

		u64 mScratchSize;
		u64 mInstanceDescsSize;
		u64 mResultSize;
	public:
		void AddInstance(ID3D12Resource* bottomLevelAS, const JMatrix& transform, u32 instanceID,   u32 hitGroupIndex);
		void ComputeASBufferSizes(bool allowUpdate, u64* scratchSize,u64* resultSize, u64* descriptorsSize);
		void Generate(
			CommandList* commandList, ID3D12Resource* scratchBuffer, ID3D12Resource* resultBuffer,  ID3D12Resource* descriptorsBuffer,					  
			bool updateOnly = false, ID3D12Resource* previousResult = nullptr);
	};

	class RayTracingShaderBindingTableGenerator
	{
	private:
		struct SBTEntry
		{
			SBTEntry(std::wstring entryPoint, std::vector<void*> inputData) :
				EntryPoint(entryPoint), InputData(inputData) {}

			const std::wstring		 EntryPoint;
			const std::vector<void*> InputData;
		};
		u32 CopyShaderData(ID3D12StateObjectProperties* raytracingPipeline,
			u8* outputData, const std::vector<SBTEntry>& shaders,
			u32 entrySize);

		u32 GetEntrySize(const std::vector<SBTEntry>& entries);

		List<SBTEntry> mRayGen;
		List<SBTEntry> mMiss;
		List<SBTEntry> mHitGroup;

		u32 mRayGenEntrySize;
		u32 mMissEntrySize;
		u32 mHitGroupEntrySize;

		u32 mProgIdSize;
	public:
		void AddRayGenerationProgram(const std::wstring& entryPoint, const std::vector<void*>& inputData);
		void AddMissProgram(const std::wstring& entryPoint, const std::vector<void*>& inputData);
		void AddHitGroup(const std::wstring& entryPoint, const std::vector<void*>& inputData);
		u32  ComputeSBTSize();
		void Generate(ID3D12Resource* sbtBuffer,
			ID3D12StateObjectProperties* raytracingPipeline);
		void Reset();
		u32 GetRayGenSectionSize() const;
		u32 GetRayGenEntrySize() const;
		u32 GetMissSectionSize() const;
		u32 GetMissEntrySize();
		u32 GetHitGroupSectionSize() const;
		u32 GetHitGroupEntrySize() const;


	};


	class RayTracingHelper
	{

	};
}