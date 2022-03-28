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

	SharedPtr<IStructuredBuffer> DirectX12TopLevelAccelerationStructure::GetPrevFrameTransformBuffer() const
	{
		return mPrevInstance;
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

		if (scratchSize > mScratchSize || mScratch == nullptr)
		{
			if (mScratch != nullptr)
			{
				mScratchSize *= 2;
			}

			DirectX12API::DestroyCommittedResource(mScratch);
			mScratch = DirectX12API::CreateCommittedResource(
				"TopLevel_ScratchBuffer",
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(mScratchSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr);
		}
		if (resultSize > mResultSize || mResult == nullptr)
		{
			if (mResult != nullptr)
			{
				mResultSize *= 2;
			}
			DirectX12API::DestroyCommittedResource(mResult);
			mResult = DirectX12API::CreateCommittedResource(
				"TopLevel_ResultBuffer",
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(mResultSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
				D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
				nullptr);

		}
		if (instanceSize > mInstanceSize || mInstance == nullptr)
		{
			if (mInstance != nullptr)
			{
				mInstanceSize *= 2;
			}
			DirectX12API::DestroyCommittedResource(mInstance);
			mInstance = DirectX12API::CreateCommittedResource(
				"TopLevel_InstanceBuffer",
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(mInstanceSize, D3D12_RESOURCE_FLAG_NONE),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr);
		}
		if (mInstance == nullptr || mResult == nullptr || mScratch == nullptr)
		{
			return;
		}
		mTopASGen.Generate(dx12Context->Get(), mScratch.Get(), mResult.Get(), mInstance.Get(), onlyUpdate, (onlyUpdate == false) ? nullptr : mResult.Get());


		const auto& instances = mTopASGen.GetInstances();
		u64 prevInstanceSize = instances.size() * sizeof(JMatrix);
		if (prevInstanceSize > mPrevInstanceSize || mPrevInstance == nullptr)
		{
			if (mPrevInstance != nullptr)
			{
				mPrevInstanceSize *= 2;
			}
			mPrevInstance = IStructuredBuffer::Create("PrevInstanceTransformStructuredBuffer", sizeof(JMatrix), mPrevInstanceSize / sizeof(JMatrix), EBufferLoadMethod::CPULoad);
		}
		if (mPrevInstance != nullptr)
		{
			u32 index = 0;
			for (auto& instance : instances)
			{
				mPrevInstance->SetDataByIndex(index, (void*)&(instance.Transform));
				++index;
			}
		}
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

		mScratchSize  = _2MB;
		mResultSize   = _2MB;
		mInstanceSize = _2MB;
		mTopASGen = TopLevelASGenerator();
	}

}