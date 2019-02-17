#pragma once
#include"DxCommon/DxCommon.h"
#include"DxCommon/DataType_Define.h"

namespace JGRC
{


	class RCORE_EXPORT FrameResource
	{

	private:
		FrameResource(const FrameResource& rhs) = delete;
		FrameResource& operator=(const FrameResource& rhs) = delete;
	public:
		FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT lightCount);
		~FrameResource() {}
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>      CmdListAlloc;
		std::unique_ptr<UploadBuffer<PassConstantData>>     PassCB     = nullptr;
		std::unique_ptr<UploadBuffer<ObjectConstantData>>   ObjectCB   = nullptr;
		std::unique_ptr<UploadBuffer<MaterialConstantData>> MaterialCB = nullptr;
		std::unique_ptr<UploadBuffer<SkinnedConstantData>>  SkinnedCB  = nullptr;
		std::unique_ptr<UploadBuffer<Light>>                LightCB    = nullptr;
		UINT64 Fence = 0;
	public:
		void ReSize(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT lightCount);
		D3D12_GPU_VIRTUAL_ADDRESS ObjectCBHeapAddress(ObjectData* Data);
		D3D12_GPU_VIRTUAL_ADDRESS PassCBHeapAddress(PassData* Data);
		D3D12_GPU_VIRTUAL_ADDRESS SkinnedCBHeapAddress(SkinnedData* Data);
	};

	class RCORE_EXPORT EngineFrameResourceManager
	{
		typedef std::vector<std::shared_ptr<FrameResource>> EngineFrameResourceArray;
	private:
		ID3D12Device* m_Device = nullptr;
		EngineFrameResourceArray m_FrameResources;
		FrameResource* m_CurrentFrameResource = nullptr;
		UINT m_CurrentFrameResourceIndex = 0;
	public:
		void BuildFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT lightCount);
		void Resize(UINT passCount, UINT objectCount,UINT materialCount, UINT lightCount);
		void FrameResourcePerFrame(ID3D12Fence* Fence);
	public:
		FrameResource* operator[](const UINT idx)   { return m_FrameResources[idx].get(); }
		UINT CurrentIndex() const                   { return m_CurrentFrameResourceIndex; }
		FrameResource* CurrentFrameResource() const { return m_CurrentFrameResource; }
	};

}