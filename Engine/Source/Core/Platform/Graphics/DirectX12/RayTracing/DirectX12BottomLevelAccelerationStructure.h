#pragma once
#include "Graphics/RayTracing/BottomLevelAccelerationStructure.h"
#include "Platform/Graphics/DirectX12/Utill/DirectX12Helper.h"

namespace JG
{

	class DirectX12BottomLevelAccelerationsStructure : public IBottomLevelAccelerationStructure
	{

		ComPtr<ID3D12Resource> mScratch;
		ComPtr<ID3D12Resource> mResult;
		u64 mScratchSize = 0;
		u64 mResultSize = 0;

		bool mIsAllowUpdate = false;
	public:
		virtual ~DirectX12BottomLevelAccelerationsStructure();
		virtual void Generate(
			SharedPtr<IComputeContext> context, SharedPtr<IVertexBuffer> vBuffer, SharedPtr<IIndexBuffer> iBuffer, bool onlyUpdate = false, bool allowUpdate = false, bool isOpaque = true) override;
	public:
		void Clear();
		ID3D12Resource* GetResult() const {
			return mResult.Get();
		}
	};



}