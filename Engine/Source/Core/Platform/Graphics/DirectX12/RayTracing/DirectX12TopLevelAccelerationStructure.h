#pragma once
#include "Graphics/RayTracing/TopLevelAccelerationStructure.h"
#include "Platform/Graphics/DirectX12/Utill/DirectX12Helper.h"
#include "Platform/Graphics/DirectX12/Utill/RayTracingHelper.h"
namespace JG
{
	class IBottomLevelAccelerationStructure;
	class DirectX12TopLevelAccelerationStructure : public ITopLevelAccelerationStructure
	{

		ComPtr<ID3D12Resource>		 mScratch;
		ComPtr<ID3D12Resource>		 mInstance;
		SharedPtr<IStructuredBuffer> mPrevInstance;
		ComPtr<ID3D12Resource> mResult;
	
		TopLevelASGenerator mTopASGen;

		


		u64 mScratchSize  = 0;
		u64 mResultSize   = 0;
		u64 mInstanceSize = 0;
	public:
		virtual ~DirectX12TopLevelAccelerationStructure();
		virtual void AddInstance(SharedPtr<IBottomLevelAccelerationStructure> btAS, const JMatrix& transform, u32 instanceID, u32 hitGroupIndex) override;
		virtual void Generate(SharedPtr<IComputeContext> context, bool onlyUpdate = false,  bool allowUpdate = false) override;
		virtual void Reset() override;
	public:
		void Clear();
		ID3D12Resource* GetResult() const {
			return mResult.Get();
		}
	};

}