#include "pch.h"
#include "RayTracingHelper.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "CommandList.h"
#ifndef ROUND_UP
#define ROUND_UP(v, powerOf2Alignment)                                         \
  (((v) + (powerOf2Alignment)-1) & ~((powerOf2Alignment)-1))
#endif
namespace JG
{
	RayTracingPipelineGenerator::Library::Library(IDxcBlob* dxil, const List<String>& exportedSymbols)
		: Dxil(dxil)
	{
		Exports.resize(exportedSymbols.size());
		ExportedSymbols.resize(exportedSymbols.size());

		for (u64 i = 0; i < ExportedSymbols.size(); ++i)
		{
			ExportedSymbols[i] = StringHelper::s2ws(exportedSymbols[i]);

			Exports[i] = {};
			Exports[i].Name = ExportedSymbols[i].c_str();
			Exports[i].ExportToRename = nullptr;
			Exports[i].Flags = D3D12_EXPORT_FLAG_NONE;
		}

		LibDesc.DXILLibrary.BytecodeLength = Dxil->GetBufferSize();
		LibDesc.DXILLibrary.pShaderBytecode = Dxil->GetBufferPointer();
		LibDesc.NumExports = ExportedSymbols.size();
		LibDesc.pExports   = Exports.data();
	}
	RayTracingPipelineGenerator::Library::Library(const Library& source)
		: Dxil(source.Dxil), ExportedSymbols(source.ExportedSymbols)
	{
		Exports.resize(ExportedSymbols.size());

		for (u64 i = 0; i < ExportedSymbols.size(); ++i)
		{
			Exports[i] = {};
			Exports[i].Name = ExportedSymbols[i].c_str();
			Exports[i].ExportToRename = nullptr;
			Exports[i].Flags = D3D12_EXPORT_FLAG_NONE;
		}

		LibDesc.DXILLibrary.BytecodeLength = Dxil->GetBufferSize();
		LibDesc.DXILLibrary.pShaderBytecode = Dxil->GetBufferPointer();
		LibDesc.NumExports = ExportedSymbols.size();
		LibDesc.pExports = Exports.data();



	}


	RayTracingPipelineGenerator::HitGroup::HitGroup(const String& hitGroupName,
		const String& closestHitSymbol,
		const String& anyHitSymbol ,
		const String& intersectionSymbol)
		: HitGroup(StringHelper::s2ws(hitGroupName), StringHelper::s2ws(closestHitSymbol),
		  StringHelper::s2ws(anyHitSymbol), StringHelper::s2ws(intersectionSymbol))
	{}
	RayTracingPipelineGenerator::HitGroup::HitGroup(
		const std::wstring& hitGroupName,
		const std::wstring& closestHitSymbol,
		const std::wstring& anyHitSymbol,
		const std::wstring& intersectionSymbol)
		: HitGroupName(hitGroupName), ClosestHitSymbol(closestHitSymbol),
		AnyHitSymbol(anyHitSymbol), IntersectionSymbol(intersectionSymbol)
	{
		Desc.HitGroupExport = HitGroupName.c_str();
		Desc.ClosestHitShaderImport = ClosestHitSymbol.empty() ? nullptr : ClosestHitSymbol.c_str();
		Desc.AnyHitShaderImport = AnyHitSymbol.empty() ? nullptr : AnyHitSymbol.c_str();
		Desc.IntersectionShaderImport =
			IntersectionSymbol.empty() ? nullptr : IntersectionSymbol.c_str();
		Desc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
	}
	RayTracingPipelineGenerator::HitGroup::HitGroup(const HitGroup& source)
		: HitGroup(source.HitGroupName, source.ClosestHitSymbol, source.AnyHitSymbol,source.IntersectionSymbol)
	{}

	RayTracingPipelineGenerator::RootSignatureAssociation::RootSignatureAssociation(
		ID3D12RootSignature* rootSignature, const std::vector<std::wstring>& symbols)
		: mRootSignature(rootSignature), mSymbols(symbols), mSymbolPointers(symbols.size())
	{
		for (size_t i = 0; i < mSymbols.size(); i++)
		{
			mSymbolPointers[i] = mSymbols[i].c_str();
		}
		mRootSignaturePointer = mRootSignature;
	}

	RayTracingPipelineGenerator::RootSignatureAssociation::RootSignatureAssociation(
		const RootSignatureAssociation& source)
		: RootSignatureAssociation(source.mRootSignature, source.mSymbols)
	{
	}

	void RayTracingPipelineGenerator::AddLibrary(IDxcBlob* dxilLib, const List<String>& symbolExports)
	{
		mLibraries.push_back(Library(dxilLib, symbolExports));
	}

	void RayTracingPipelineGenerator::AddHitGroup(const String& hitGroupName,
		const String& closestHitSymbol,
		const String& anyHitSymbol /*= L""*/,
		const String& intersectionSymbol /*= L""*/)
	{
		mHitGroups.emplace_back(
			HitGroup(hitGroupName, closestHitSymbol, anyHitSymbol, intersectionSymbol));
	}

	void RayTracingPipelineGenerator::SetRootSignature(ID3D12RootSignature* rootSig)
	{
		mGlobalRootSignature = rootSig;
	}

	void RayTracingPipelineGenerator::AddLocalRootSignature(ID3D12RootSignature* rootSig, const List<String>& symbols)
	{
		mRootSignatureAssociations.push_back(RootSignatureAssociation(rootSig, StringHelper::s2ws(symbols)));
	}

	void RayTracingPipelineGenerator::SetMaxPayloadSize(u32 btSize)
	{
		mMaxPayLoadSize = btSize;
	}

	void RayTracingPipelineGenerator::SetMaxAttributeSize(u32 btSize)
	{
		mMaxAttributeSize = btSize;
	}

	void RayTracingPipelineGenerator::SetMaxRecursionDepth(u32 maxDepth)
	{
		mMaxRecursionDepth = maxDepth;
	}

	ComPtr<ID3D12StateObject> RayTracingPipelineGenerator::Generate()
	{
		u64 subObjectCount =
			mLibraries.size() + mHitGroups.size() 
			+ 1 // GlobalShaderRootSignature
			+ (2 * mRootSignatureAssociations.size())
			+ 1 // ShaderConfig
			+ 1; // PipelineConfig


		List<D3D12_STATE_SUBOBJECT> subObjects(subObjectCount);

		u64 currIndex = 0;


		for (const Library& lib : mLibraries)
		{
			D3D12_STATE_SUBOBJECT libSubobject = {};
			libSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
			libSubobject.pDesc = &lib.LibDesc;

			subObjects[currIndex++] = libSubobject;
		}
		for (const HitGroup& group : mHitGroups)
		{
			D3D12_STATE_SUBOBJECT hitGroup = {};
			hitGroup.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
			hitGroup.pDesc = &group.Desc;

			subObjects[currIndex++] = hitGroup;
		}

		// Global RootSig
		D3D12_STATE_SUBOBJECT globalRootSig;
		globalRootSig.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
		globalRootSig.pDesc = mGlobalRootSignature.GetAddressOf();
		subObjects[currIndex++] = globalRootSig;

		// Local RootSig
		for (RootSignatureAssociation& assoc : mRootSignatureAssociations)
		{
			D3D12_STATE_SUBOBJECT rootSigObject = {};
			rootSigObject.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
			rootSigObject.pDesc = &assoc.mRootSignature;
			subObjects[currIndex++] = rootSigObject;


			assoc.mAssociation.NumExports = static_cast<UINT>(assoc.mSymbolPointers.size());
			assoc.mAssociation.pExports = assoc.mSymbolPointers.data();
			assoc.mAssociation.pSubobjectToAssociate = &subObjects[(currIndex - 1)];

			D3D12_STATE_SUBOBJECT rootSigAssociationObject = {};
			rootSigAssociationObject.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
			rootSigAssociationObject.pDesc = &assoc.mAssociation;

			subObjects[currIndex++] = rootSigAssociationObject;
		}

		// Shader Config 
		D3D12_RAYTRACING_SHADER_CONFIG shaderConfigDesc = {};
		shaderConfigDesc.MaxPayloadSizeInBytes = mMaxPayLoadSize;
		shaderConfigDesc.MaxAttributeSizeInBytes = mMaxAttributeSize;

		D3D12_STATE_SUBOBJECT shaderConfig = {};
		shaderConfig.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
		shaderConfig.pDesc = &shaderConfigDesc;

		subObjects[currIndex++] = shaderConfig;

		// Pipeline Config
		D3D12_RAYTRACING_PIPELINE_CONFIG pipelineConfig = {};
		pipelineConfig.MaxTraceRecursionDepth = mMaxRecursionDepth;

		D3D12_STATE_SUBOBJECT pipelineConfigObject = {};
		pipelineConfigObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
		pipelineConfigObject.pDesc = &pipelineConfig;

		subObjects[currIndex++] = pipelineConfigObject;




		D3D12_STATE_OBJECT_DESC pipelineDesc = {};
		pipelineDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
		pipelineDesc.NumSubobjects = currIndex; 
		pipelineDesc.pSubobjects   = subObjects.data();


		ID3D12StateObject* rtStateObject = nullptr;
		HRESULT hr = DirectX12API::GetD3DDevice()->CreateStateObject(&pipelineDesc, IID_PPV_ARGS(&rtStateObject));
		if (FAILED(hr))
		{
			throw std::logic_error("Could not create the raytracing state object");
		}
		return rtStateObject;
	}




	void BottomLevelASGenerator::AddVertexBuffer(
		ID3D12Resource* vertexBuffer, u64 vertexOffsetInBytes, u32 vertexCount, u32 vertexSizeInBytes,
		ID3D12Resource* transformBuffer, u64 transformOffsetInBytes, bool isOpaque)
	{
		AddVertexBuffer(vertexBuffer, vertexOffsetInBytes, vertexCount,
			vertexSizeInBytes, nullptr, 0, 0, transformBuffer,
			transformOffsetInBytes, isOpaque);
	}

	void BottomLevelASGenerator::AddVertexBuffer(
		ID3D12Resource* vertexBuffer, u64 vertexOffsetInBytes, u32 vertexCount, u32 vertexSizeInBytes,
		ID3D12Resource* indexBuffer, u64 indexOffsetInBytes, u32 indexCount,
		ID3D12Resource* transformBuffer, u64 transformOffsetInBytes, bool isOpaque)
	{
		D3D12_RAYTRACING_GEOMETRY_DESC descriptor = {};
		descriptor.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
		descriptor.Triangles.VertexBuffer.StartAddress  = vertexBuffer->GetGPUVirtualAddress() + vertexOffsetInBytes;
		descriptor.Triangles.VertexBuffer.StrideInBytes = vertexSizeInBytes;
		descriptor.Triangles.VertexCount  = vertexCount;
		descriptor.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		descriptor.Triangles.IndexBuffer  = indexBuffer ? (indexBuffer->GetGPUVirtualAddress() + indexOffsetInBytes) : 0;
		descriptor.Triangles.IndexFormat  = indexBuffer ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_UNKNOWN;
		descriptor.Triangles.IndexCount   = indexCount;
		descriptor.Triangles.Transform3x4 = transformBuffer ? (transformBuffer->GetGPUVirtualAddress() + transformOffsetInBytes) : 0;
		descriptor.Flags = isOpaque ? D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE : D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;

		mVertexBuffers.push_back(descriptor);
	}






	void BottomLevelASGenerator::ComputeASBufferSizes(bool allowUpdate, UINT64* scratchSize, UINT64* resultSize)
	{
		mFlags = allowUpdate ? D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE : D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;


		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS prebuildDesc;
		prebuildDesc.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		prebuildDesc.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		prebuildDesc.NumDescs = static_cast<u32>(mVertexBuffers.size());
		prebuildDesc.pGeometryDescs = mVertexBuffers.data();
		prebuildDesc.Flags = mFlags;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
		DirectX12API::GetD3DDevice()->GetRaytracingAccelerationStructurePrebuildInfo(&prebuildDesc, &info);

		// Buffer sizes need to be 256-byte-aligned
		*scratchSize = ROUND_UP(info.ScratchDataSizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		*resultSize  = ROUND_UP(info.ResultDataMaxSizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

		// Store the memory requirements for use during build
		mScratchSize = *scratchSize;
		mResultSize = *resultSize;
	}

	void BottomLevelASGenerator::Generate(
		CommandList* commandList,ID3D12Resource* scratchBuffer, ID3D12Resource* resultBuffer, 
		bool updateOnly, ID3D12Resource* previousResult)
	{
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS flags = mFlags;

		if (flags == D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE && updateOnly) {
			flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
		}

		// Sanity checks
		if (mFlags != D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE && updateOnly) {
			throw std::logic_error("Cannot update a bottom-level AS not originally built for updates");
		}
		if (updateOnly && previousResult == nullptr) {
			throw std::logic_error("Bottom-level hierarchy update requires the previous hierarchy");
		}

		if (mResultSize == 0 || mScratchSize == 0) {
			throw std::logic_error(
				"Invalid scratch and result buffer sizes - ComputeASBufferSizes needs "
				"to be called before Build");
		}

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc;
		buildDesc.Inputs.Type		 = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		buildDesc.Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		buildDesc.Inputs.NumDescs	 = static_cast<UINT>(mVertexBuffers.size());
		buildDesc.Inputs.pGeometryDescs = mVertexBuffers.data();
		buildDesc.DestAccelerationStructureData    = { resultBuffer->GetGPUVirtualAddress() };
		buildDesc.ScratchAccelerationStructureData = { scratchBuffer->GetGPUVirtualAddress() };
		buildDesc.SourceAccelerationStructureData  = previousResult ? previousResult->GetGPUVirtualAddress() : 0;
		buildDesc.Inputs.Flags = flags;

		// Build the AS
		commandList->Get()->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
		commandList->UAVBarrier(resultBuffer, true);
	}



	void TopLevelASGenerator::AddInstance(ID3D12Resource* bottomLevelAS, const JMatrix& transform, u32 instanceID, u32 hitGroupIndex, u32 instanceMask)
	{
		mInstances.emplace_back(Instance(bottomLevelAS, transform, instanceID, hitGroupIndex, instanceMask));
	}
	void TopLevelASGenerator::ComputeASBufferSizes(bool allowUpdate, u64* scratchSize, u64* resultSize, u64* descriptorsSize)
	{
		mFlags = allowUpdate ? D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE : D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;


		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS prebuildDesc = {};
		prebuildDesc.Type		 = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		prebuildDesc.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		prebuildDesc.NumDescs	 = static_cast<u32>(mInstances.size());
		prebuildDesc.Flags		 = mFlags;


		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
		DirectX12API::GetD3DDevice()->GetRaytracingAccelerationStructurePrebuildInfo(&prebuildDesc, &info);


		info.ResultDataMaxSizeInBytes = ROUND_UP(info.ResultDataMaxSizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		info.ScratchDataSizeInBytes   = ROUND_UP(info.ScratchDataSizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

		mResultSize  = info.ResultDataMaxSizeInBytes;
		mScratchSize = info.ScratchDataSizeInBytes;

		mInstanceDescsSize = ROUND_UP(sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * static_cast<u64>(mInstances.size()), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

		*scratchSize = mScratchSize;
		*resultSize = mResultSize;
		*descriptorsSize = mInstanceDescsSize;
	}
	void TopLevelASGenerator::Generate(
		CommandList* commandList, ID3D12Resource* scratchBuffer, ID3D12Resource* resultBuffer, ID3D12Resource* descriptorsBuffer,
		bool updateOnly, ID3D12Resource* previousResult)
	{

		D3D12_RAYTRACING_INSTANCE_DESC* instanceDescs;
		descriptorsBuffer->Map(0, nullptr, reinterpret_cast<void**>(&instanceDescs));
		if (!instanceDescs)
		{
			JG_LOG_ERROR("Cannot map the instance descriptor buffer - is it "
				"in the upload heap?");
			descriptorsBuffer->Unmap(0, nullptr);
			return;
		}

		auto instanceCount = static_cast<UINT>(mInstances.size());


		if (!updateOnly)
		{
			ZeroMemory(instanceDescs, mInstanceDescsSize);
		}

		for (uint32_t i = 0; i < instanceCount; i++)
		{
			instanceDescs[i].InstanceID = mInstances[i].InstanceID;
			instanceDescs[i].InstanceContributionToHitGroupIndex = mInstances[i].HitGroupIndex;
			instanceDescs[i].Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;

		
			memcpy(instanceDescs[i].Transform, &(mInstances[i].Transform), sizeof(instanceDescs[i].Transform));
			instanceDescs[i].AccelerationStructure = mInstances[i].BottomLevelAS->GetGPUVirtualAddress();
			instanceDescs[i].InstanceMask = mInstances[i].InstanceMask;
		}

		descriptorsBuffer->Unmap(0, nullptr);

		D3D12_GPU_VIRTUAL_ADDRESS pSourceAS = updateOnly ? previousResult->GetGPUVirtualAddress() : 0;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS flags = mFlags;
		if (flags == D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE && updateOnly)
		{
			flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
		}

		// Sanity checks
		if (mFlags != D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE && updateOnly)
		{
			throw std::logic_error("Cannot update a top-level AS not originally built for updates");
		}
		if (updateOnly && previousResult == nullptr)
		{
			throw std::logic_error("Top-level hierarchy update requires the previous hierarchy");
		}

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
		buildDesc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		buildDesc.Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		buildDesc.Inputs.InstanceDescs = descriptorsBuffer->GetGPUVirtualAddress();
		buildDesc.Inputs.NumDescs = instanceCount;
		buildDesc.DestAccelerationStructureData = { resultBuffer->GetGPUVirtualAddress()};
		buildDesc.ScratchAccelerationStructureData = { scratchBuffer->GetGPUVirtualAddress()};
		buildDesc.SourceAccelerationStructureData = pSourceAS;
		buildDesc.Inputs.Flags = flags;

		// Build the top-level AS
		commandList->Get()->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
		commandList->UAVBarrier(resultBuffer, true);
	}

	const List<TopLevelASGenerator::Instance>& TopLevelASGenerator::GetInstances() const
	{
		return mInstances;
	}



	u32 RayTracingShaderBindingTableGenerator::CopyShaderData(ID3D12StateObjectProperties* raytracingPipeline, u8* outputData, const std::vector<SBTEntry>& shaders, u32 entrySize)
	{
		u8* pData = outputData;
		for (const auto& shader : shaders)
		{
			void* id = raytracingPipeline->GetShaderIdentifier(shader.EntryPoint.c_str());
			if (!id)
			{
				std::wstring errMsg(std::wstring(L"Unknown shader identifier used in the SBT: ") +
					shader.EntryPoint);
				throw std::logic_error(std::string(errMsg.begin(), errMsg.end()));
			}
			memcpy(pData, id, mProgIdSize);
			memcpy(pData + mProgIdSize, shader.InputData, shader.InputDataSize);

			pData += entrySize;
		}
		return static_cast<u32>(shaders.size()) * entrySize;
	}

	u32 RayTracingShaderBindingTableGenerator::GetEntrySize(const std::vector<SBTEntry>& entries)
	{
		u64 maxArgSize = 0;
		for (const auto& shader : entries)
		{
			maxArgSize = Math::Max(maxArgSize, shader.InputDataSize);
		}
		u32 entrySize = mProgIdSize + static_cast<u32>(maxArgSize);
		entrySize = ROUND_UP(entrySize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);

		return entrySize;
	}

	void RayTracingShaderBindingTableGenerator::AddRayGenerationProgram(const std::wstring& entryPoint, const void* inputData, u64 inputDataSize)
	{
		mRayGen.emplace_back(SBTEntry(entryPoint, inputData, inputDataSize));
	}
	void RayTracingShaderBindingTableGenerator::AddMissProgram(const std::wstring& entryPoint, const void* inputData, u64 inputDataSize)
	{
		mMiss.emplace_back(SBTEntry(entryPoint, inputData, inputDataSize));
	}
	void RayTracingShaderBindingTableGenerator::AddHitGroup(const std::wstring& entryPoint, const void* inputData, u64 inputDataSize)
	{
		mHitGroup.emplace_back(SBTEntry(entryPoint, inputData, inputDataSize));
	}
	u32  RayTracingShaderBindingTableGenerator::ComputeSBTSize()
	{
		mProgIdSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		mRayGenEntrySize   = GetEntrySize(mRayGen);
		mMissEntrySize     = GetEntrySize(mMiss);
		mHitGroupEntrySize = GetEntrySize(mHitGroup);

		u32 sbtSize = ROUND_UP(mRayGenEntrySize * static_cast<u32>(mRayGen.size()) +
			mMissEntrySize * static_cast<u32>(mMiss.size()) +
			mHitGroupEntrySize * static_cast<u32>(mHitGroup.size()),
			256);
		return sbtSize;
	}
	void RayTracingShaderBindingTableGenerator::Generate(
		ID3D12Resource* sbtBuffer,
		ID3D12StateObjectProperties* raytracingPipeline)
	{
		u8* pData;
		HRESULT hr = sbtBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
		if (FAILED(hr))
		{
			throw std::logic_error("Could not map the shader binding table");
		}

		u32 offset = 0;

		offset = CopyShaderData(raytracingPipeline, pData, mRayGen, mRayGenEntrySize);
		pData += offset;

		offset = CopyShaderData(raytracingPipeline, pData, mMiss, mMissEntrySize);
		pData += offset;

		offset = CopyShaderData(raytracingPipeline, pData, mHitGroup, mHitGroupEntrySize);

		sbtBuffer->Unmap(0, nullptr);
	}
	void RayTracingShaderBindingTableGenerator::Reset()
	{
		mRayGen.clear();
		mMiss.clear();
		mHitGroup.clear();

		mRayGenEntrySize = 0;
		mMissEntrySize = 0;
		mHitGroupEntrySize = 0;
		mProgIdSize = 0;
	}
	u32 RayTracingShaderBindingTableGenerator::GetRayGenSectionSize() const
	{
		return mRayGenEntrySize * static_cast<u32>(mRayGen.size());
	}
	u32 RayTracingShaderBindingTableGenerator::GetRayGenEntrySize() const
	{
		return mRayGenEntrySize;
	}
	u32 RayTracingShaderBindingTableGenerator::GetMissSectionSize() const
	{
		return mMissEntrySize * static_cast<UINT>(mMiss.size());
	}
	u32 RayTracingShaderBindingTableGenerator::GetMissEntrySize()
	{
		return mMissEntrySize;
	}
	u32 RayTracingShaderBindingTableGenerator::GetHitGroupSectionSize() const
	{
		return mHitGroupEntrySize * static_cast<UINT>(mHitGroup.size());
	}
	u32 RayTracingShaderBindingTableGenerator::GetHitGroupEntrySize() const
	{
		return mHitGroupEntrySize;
	}
}