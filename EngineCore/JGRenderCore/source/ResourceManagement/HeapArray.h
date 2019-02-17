#pragma once
#include"DxCommon/DxCommon.h"


namespace JGRC
{
	class DxDevice;
	enum class EHeapType
	{
		SRV,
		UAV,
		RTV,
		DSV,
		CBV
	};
	class HeapData
	{
		UINT64 m_ID;
	public:
		UINT64 ID() const {
			return m_ID;
		}
		UINT64 SetID(UINT64 id) {
			m_ID = id;
		}
	};
	class CbvData : public HeapData
	{
	public:
		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;
	};
	class SrvData : public HeapData
	{
	public:
		UINT64 ID;
		ID3D12Resource* Resource = nullptr;
		D3D12_SHADER_RESOURCE_VIEW_DESC Desc;
	};
	class UavData : public HeapData
	{
	public:
		ID3D12Resource* Resource = nullptr;
		ID3D12Resource* CounterResource = nullptr;
		D3D12_UNORDERED_ACCESS_VIEW_DESC Desc;
	};
	class RtvData : public HeapData
	{
	public:
		ID3D12Resource* Resource = nullptr;
		D3D12_RENDER_TARGET_VIEW_DESC Desc;
	};
	class DsvData : public HeapData
	{
	public:
		ID3D12Resource* Resource = nullptr;
		D3D12_DEPTH_STENCIL_VIEW_DESC Desc;
	};
	template<EHeapType HeapType, typename DataType>
	class RCORE_EXPORT HeapArray
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
		std::unordered_map<DataType*, UINT> m_HeapDataIndex;
		std::queue<UINT>                    m_DeadHeapQue;
		//
		UINT64    m_HeapSize   = 0;
		UINT64    m_HeapOffset = -1;
		DxDevice* m_Device = nullptr;
	public:
		HeapArray(DxDevice* Device, UINT Size = 100);
		~HeapArray() = default;
	public:
		void Push_Back(DataType* Data);
		void Modify(DataType* Data);
		void ReSize(UINT Size);
		void Remove(DataType* Data);
		void Clear();
		bool Empty();
		UINT64 Size();
		UINT64 MemSize();
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle(DataType* Data);
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle(DataType* Data);
	private:
		D3D12_DESCRIPTOR_HEAP_DESC    GetDesc(UINT Size);
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT Index);
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT Index);
	};

}