#include "pch.h"
#include "DirectX12TopLevelAccelerationStructure.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "DirectX12BottomLevelAccelerationStructure.h"
#include "Platform/Graphics/DirectX12/Utill/CommandList.h"
namespace JG
{
	DirectX12TopLevelAccelerationStructure::~DirectX12TopLevelAccelerationStructure()
	{
		Clear();
	}

	void DirectX12TopLevelAccelerationStructure::AddInstance(SharedPtr<IBottomLevelAccelerationStructure> btAS, const JMatrix& transform, u32 instanceID, u32 hitGroupIndex)
	{
		auto dx12BLAS = static_cast<DirectX12BottomLevelAccelerationsStructure*>(btAS.get());
		mTopASGen.AddInstance(dx12BLAS->GetResult(), transform, instanceID, hitGroupIndex);
	}

	void DirectX12TopLevelAccelerationStructure::Generate(SharedPtr<IComputeContext> context, bool onlyUpdate , bool allowUpdate)
	{

		u64 scratchSize  = 0;
		u64 resultSize   = 0;
		u64 instanceSize = 0;
		auto dx12Context = static_cast<DirectX12ComputeContext*>(context.get());


		mTopASGen.ComputeASBufferSizes(allowUpdate, &scratchSize, &resultSize, &instanceSize);

		if (scratchSize > mScratchSize)
		{
			mScratchSize = scratchSize;
			DirectX12API::DestroyCommittedResource(mScratch);
			mScratch = DirectX12API::CreateCommittedResource(
				"TopLevel_ScratchBuffer",
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(scratchSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr);
		}
		if (resultSize > mResultSize)
		{
			mResultSize = resultSize;
			DirectX12API::DestroyCommittedResource(mResult);
			mResult = DirectX12API::CreateCommittedResource(
				"TopLevel_ResultBuffer",
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(resultSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
				D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
				nullptr);

		}
		if (instanceSize > mInstanceSize)
		{
			mInstanceSize = instanceSize;
			DirectX12API::DestroyCommittedResource(mInstance);
			mInstance = DirectX12API::CreateCommittedResource(
				"TopLevel_InstanceBuffer",
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(instanceSize, D3D12_RESOURCE_FLAG_NONE),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr);
		}
		if (mInstance == nullptr || mResult == nullptr || mScratch == nullptr)
		{
			return;
		}
		mTopASGen.Generate(dx12Context->Get(), mScratch.Get(), mResult.Get(), mInstance.Get(), onlyUpdate, (onlyUpdate == false) ? nullptr : mResult.Get());
	}

	void DirectX12TopLevelAccelerationStructure::Reset()
	{
		mTopASGen = TopLevelASGenerator();
	}

	void DirectX12TopLevelAccelerationStructure::Clear()
	{
		DirectX12API::DestroyCommittedResource(mScratch);
		DirectX12API::DestroyCommittedResource(mInstance);
		DirectX12API::DestroyCommittedResource(mResult);

		mScratchSize  = 0;
		mResultSize   = 0;
		mInstanceSize = 0;
		mTopASGen = TopLevelASGenerator();
	}

}