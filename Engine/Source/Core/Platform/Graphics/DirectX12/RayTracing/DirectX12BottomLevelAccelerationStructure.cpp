#include "pch.h"
#include "DirectX12BottomLevelAccelerationStructure.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "Platform/Graphics/DirectX12/Utill/RayTracingHelper.h"
#include "Platform/Graphics/DirectX12/DirectX12Resource.h"
#include "Platform/Graphics/DirectX12/Utill/CommandList.h"
namespace JG
{




	DirectX12BottomLevelAccelerationsStructure::~DirectX12BottomLevelAccelerationsStructure()
	{
		Clear();
	}

	void DirectX12BottomLevelAccelerationsStructure::Generate(
		SharedPtr<IComputeContext> context, SharedPtr<IVertexBuffer> vBuffer, SharedPtr<IIndexBuffer> iBuffer,
		bool onlyUpdate, bool allowUpdate, bool isOpaque)
	{

		if (context == nullptr || vBuffer == nullptr || iBuffer == nullptr)
		{
			return;
		}
		auto dx12VBuffer = static_cast<DirectX12VertexBuffer*>(vBuffer.get());
		auto dx12IBuffer = static_cast<DirectX12IndexBuffer*>(iBuffer.get());
		auto dx12Context = static_cast<DirectX12ComputeContext*>(context.get());




		BottomLevelASGenerator btASGen;
		btASGen.AddVertexBuffer(
			dx12VBuffer->Get(), 0, dx12VBuffer->GetVertexCount(), dx12VBuffer->GetVertexSize(), 
			dx12IBuffer->Get(), 0, dx12IBuffer->GetIndexCount(), nullptr, 0, isOpaque);


		u64 scratchSize = 0;
		u64 resultSize = 0;
		btASGen.ComputeASBufferSizes(allowUpdate, &scratchSize, &resultSize);




		if (scratchSize > mScratchSize)
		{
			DirectX12API::DestroyCommittedResource(mScratch);
			mScratch = DirectX12API::CreateCommittedResource(
				"BottomAS_ScratchBuffer",
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(scratchSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr);
			mScratchSize = scratchSize;
		}
		if (resultSize > mResultSize)
		{
			DirectX12API::DestroyCommittedResource(mResult);
			mResult = DirectX12API::CreateCommittedResource(
				"BottomAS_ResultBuffer",
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(resultSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
				D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
				nullptr);
			mResultSize = resultSize;
		}


		mIsAllowUpdate = allowUpdate;

		btASGen.Generate(dx12Context->Get(), mScratch.Get(), mResult.Get(), onlyUpdate, (onlyUpdate == false) ? nullptr : mResult.Get());
	}
	void DirectX12BottomLevelAccelerationsStructure::Clear()
	{
		DirectX12API::DestroyCommittedResource(mResult);
		DirectX12API::DestroyCommittedResource(mScratch);
		mScratchSize = 0;
		mResultSize = 0;
		mIsAllowUpdate = false;
	}

}