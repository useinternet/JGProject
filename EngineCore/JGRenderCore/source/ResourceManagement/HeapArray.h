#pragma once
#include"DxCommon/DxCommon.h"
#include"DxCore/DxDevice.h"
namespace JGRC
{
	class DxDevice;
	enum class EHeapType
	{
		SRV,
		UAV,
		RTV,
		DSV,
		CBV,
		NONE
	};
	template<EHeapType HeapType, typename DataType>
	class HeapArray;


	template<EHeapType HeapType, typename DataType>
	class HeapData
	{
		friend HeapArray<HeapType, DataType>;
		UINT m_ID;
		CD3DX12_GPU_DESCRIPTOR_HANDLE m_GPUHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_CPUHandle;
	public:
		UINT ID() const {
			return m_ID;
		}
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle() {
			return m_GPUHandle;
		}
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle() {
			return m_CPUHandle;
		}
	};

	class CbvData : public HeapData<EHeapType::CBV, CbvData>
	{
	public:
		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;
	};
	class SrvData : public HeapData<EHeapType::SRV, SrvData>
	{
	public:
		ID3D12Resource* Resource = nullptr;
		D3D12_SHADER_RESOURCE_VIEW_DESC Desc;
	};
	class UavData : public HeapData<EHeapType::UAV, UavData>
	{
	public:
		ID3D12Resource* Resource = nullptr;
		ID3D12Resource* CounterResource = nullptr;
		D3D12_UNORDERED_ACCESS_VIEW_DESC Desc;
	};
	class RtvData : public HeapData<EHeapType::RTV, RtvData>
	{
	public:
		ID3D12Resource* Resource = nullptr;
		D3D12_RENDER_TARGET_VIEW_DESC Desc;
	};
	class DsvData : public HeapData<EHeapType::DSV, DsvData>
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
		UINT    m_HeapSize   = 0;
		UINT    m_HeapOffset = -1;
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
		ID3D12DescriptorHeap* GetHeap();
	private:
		D3D12_DESCRIPTOR_HEAP_DESC    GetDesc(UINT Size);
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT Index);
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT Index);
	};


	template<EHeapType HeapType, typename DataType>
	HeapArray<HeapType, DataType>::HeapArray(DxDevice* Device, UINT Size)
	{
		m_HeapSize = Size;
		m_Device = Device;
		ThrowIfFailed(m_Device->Get()->CreateDescriptorHeap(
			&GetDesc(m_HeapSize),
			IID_PPV_ARGS(m_Heap.GetAddressOf())));
	}

	template<EHeapType HeapType, typename DataType>
	void HeapArray<HeapType, DataType>::Push_Back(DataType* Data) { }
	template<>
	void HeapArray<EHeapType::CBV, CbvData>::Push_Back(CbvData* Data)
	{
		if (m_HeapDataIndex.find(Data) != m_HeapDataIndex.end())
		{
			// Error 로그
			return;
		}
		if (m_HeapOffset >= m_HeapSize)
		{
			ReSize(m_HeapSize * 2);
		}
		// Dead 힙이 존재하면 해당 Offset에 힙 메모리 할당
		UINT HeapOffset = 0;
		if (m_DeadHeapQue.empty())
		{
			HeapOffset = ++m_HeapOffset;
		}
		else
		{
			HeapOffset = m_DeadHeapQue.front();
			m_DeadHeapQue.pop();
		}

		m_Device->Get()->CreateConstantBufferView(
			&Data->Desc,
			GetCPUHandle(m_HeapOffset));


		Data->m_ID = m_HeapOffset;
		Data->m_CPUHandle = GetCPUHandle(m_HeapOffset);
		Data->m_GPUHandle = GetGPUHandle(m_HeapOffset);
		m_HeapDataIndex[Data] = m_HeapOffset;
	}
	template<>
	void HeapArray<EHeapType::SRV, SrvData>::Push_Back(SrvData* Data)
	{
		if (m_HeapDataIndex.find(Data) != m_HeapDataIndex.end())
		{
			// Error 로그
			return;
		}
		if (m_HeapOffset >= m_HeapSize)
		{
			ReSize(m_HeapSize * 2);
		}
		// Dead 힙이 존재하면 해당 Offset에 힙 메모리 할당
		UINT HeapOffset = 0;
		if (m_DeadHeapQue.empty())
		{
			HeapOffset = ++m_HeapOffset;
		}
		else
		{
			HeapOffset = m_DeadHeapQue.front();
			m_DeadHeapQue.pop();
		}

		m_Device->Get()->CreateShaderResourceView(
			Data->Resource,
			&Data->Desc,
			GetCPUHandle(m_HeapOffset));

		Data->m_ID = m_HeapOffset;
		Data->m_CPUHandle = GetCPUHandle(m_HeapOffset);
		Data->m_GPUHandle = GetGPUHandle(m_HeapOffset);
		m_HeapDataIndex[Data] = m_HeapOffset;
	}
	template<>
	void HeapArray<EHeapType::UAV, UavData>::Push_Back(UavData* Data)
	{
		if (m_HeapDataIndex.find(Data) != m_HeapDataIndex.end())
		{
			// Error 로그
			return;
		}
		if (m_HeapOffset >= m_HeapSize)
		{
			ReSize(m_HeapSize * 2);
		}
		// Dead 힙이 존재하면 해당 Offset에 힙 메모리 할당
		UINT HeapOffset = 0;
		if (m_DeadHeapQue.empty())
		{
			HeapOffset = ++m_HeapOffset;
		}
		else
		{
			HeapOffset = m_DeadHeapQue.front();
			m_DeadHeapQue.pop();
		}

		m_Device->Get()->CreateUnorderedAccessView(
			Data->Resource,
			Data->CounterResource,
			&Data->Desc,
			GetCPUHandle(m_HeapOffset));

		Data->m_ID = m_HeapOffset;
		Data->m_CPUHandle = GetCPUHandle(m_HeapOffset);
		Data->m_GPUHandle = GetGPUHandle(m_HeapOffset);
		m_HeapDataIndex[Data] = m_HeapOffset;
	}
	template<>
	void HeapArray<EHeapType::RTV, RtvData>::Push_Back(RtvData* Data)
	{
		if (m_HeapDataIndex.find(Data) != m_HeapDataIndex.end())
		{
			// Error 로그
			return;
		}
		if (m_HeapOffset >= m_HeapSize)
		{
			ReSize(m_HeapSize * 2);
		}
		// Dead 힙이 존재하면 해당 Offset에 힙 메모리 할당
		UINT HeapOffset = 0;
		if (m_DeadHeapQue.empty())
		{
			HeapOffset = ++m_HeapOffset;
		}
		else
		{
			HeapOffset = m_DeadHeapQue.front();
			m_DeadHeapQue.pop();
		}

		m_Device->Get()->CreateRenderTargetView(
			Data->Resource,
			&Data->Desc,
			GetCPUHandle(m_HeapOffset));

		Data->m_ID = m_HeapOffset;
		Data->m_CPUHandle = GetCPUHandle(m_HeapOffset);
		Data->m_GPUHandle = GetGPUHandle(m_HeapOffset);
		m_HeapDataIndex[Data] = m_HeapOffset;
	}
	template<>
	void HeapArray<EHeapType::DSV, DsvData>::Push_Back(DsvData* Data)
	{
		if (m_HeapDataIndex.find(Data) != m_HeapDataIndex.end())
		{
			// Error 로그
			return;
		}
		if (m_HeapOffset >= m_HeapSize)
		{
			ReSize(m_HeapSize * 2);
		}
		// Dead 힙이 존재하면 해당 Offset에 힙 메모리 할당
		UINT HeapOffset = 0;
		if (m_DeadHeapQue.empty())
		{
			HeapOffset = ++m_HeapOffset;
		}
		else
		{
			HeapOffset = m_DeadHeapQue.front();
			m_DeadHeapQue.pop();
		}

		m_Device->Get()->CreateDepthStencilView(
			Data->Resource,
			&Data->Desc,
			GetCPUHandle(m_HeapOffset));

		Data->m_ID = m_HeapOffset;
		Data->m_CPUHandle = GetCPUHandle(m_HeapOffset);
		Data->m_GPUHandle = GetGPUHandle(m_HeapOffset);
		m_HeapDataIndex[Data] = m_HeapOffset;
	}
	template<EHeapType HeapType, typename DataType>
	void HeapArray<HeapType, DataType>::Modify(DataType* Data) { }
	template<>
	void HeapArray<EHeapType::CBV, CbvData>::Modify(CbvData* Data)
	{
		if (m_HeapDataIndex.find(Data) == m_HeapDataIndex.end())
		{
			// Error 검출
			return;
		}
		m_Device->Get()->CreateConstantBufferView(
			&Data->Desc,
			Data->CPUHandle());
	}
	template<>
	void HeapArray<EHeapType::SRV, SrvData>::Modify(SrvData* Data)
	{
		if (m_HeapDataIndex.find(Data) == m_HeapDataIndex.end())
		{
			// Error 검출
			return;
		}
		m_Device->Get()->CreateShaderResourceView(
			Data->Resource,
			&Data->Desc,
			Data->CPUHandle());
	}
	template<>
	void HeapArray<EHeapType::UAV, UavData>::Modify(UavData* Data)
	{
		if (m_HeapDataIndex.find(Data) == m_HeapDataIndex.end())
		{
			// Error 검출
			return;
		}
		m_Device->Get()->CreateUnorderedAccessView(
			Data->Resource,
			Data->CounterResource,
			&Data->Desc,
			Data->CPUHandle());
	}
	template<>
	void HeapArray<EHeapType::RTV, RtvData>::Modify(RtvData* Data)
	{
		if (m_HeapDataIndex.find(Data) == m_HeapDataIndex.end())
		{
			// Error 검출
			return;
		}
		m_Device->Get()->CreateRenderTargetView(
			Data->Resource,
			&Data->Desc,
			Data->CPUHandle());
	}
	template<>
	void HeapArray<EHeapType::DSV, DsvData>::Modify(DsvData* Data)
	{
		if (m_HeapDataIndex.find(Data) == m_HeapDataIndex.end())
		{
			// Error 검출
			return;
		}
		m_Device->Get()->CreateDepthStencilView(
			Data->Resource,
			&Data->Desc,
			Data->CPUHandle());
	}
	template<EHeapType HeapType, typename DataType>
	void HeapArray<HeapType, DataType>::ReSize(UINT Size)
	{
		m_Heap.Reset();
		if (m_HeapSize < (UINT)m_HeapDataIndex.size())
			m_HeapSize = (UINT)m_HeapDataIndex.size();
		else
			m_HeapSize = Size;


		ThrowIfFailed(m_Device->Get()->CreateDescriptorHeap(
			&GetDesc(m_HeapSize),
			IID_PPV_ARGS(m_Heap.GetAddressOf())));
		for (auto& Data : m_HeapDataIndex)
		{
			Modify(Data.first);
		}
	}
	template<EHeapType HeapType, typename DataType>
	void HeapArray<HeapType, DataType>::Remove(DataType* Data)
	{
		if (m_HeapDataIndex.find(Data) == m_HeapDataIndex.end())
		{
			// Error 검출
			return;
		}
		m_HeapDataIndex.erase(Data);
		m_DeadHeapQue.push(Data->ID());
	}
	template<EHeapType HeapType, typename DataType>
	void HeapArray<HeapType, DataType>::Clear()
	{
		m_HeapDataIndex.clear();
		while (!m_DeadHeapQue.empty())
			m_DeadHeapQue.pop();
		m_HeapOffset = -1;
		m_Heap.Reset();
		ReSize(100);
	}
	template<EHeapType HeapType, typename DataType>
	bool HeapArray<HeapType, DataType>::Empty()
	{
		return m_HeapOffset == -1;
	}
	template<EHeapType HeapType, typename DataType>
	UINT64 HeapArray<HeapType, DataType>::Size()
	{
		return m_HeapOffset + 1;
	}
	template<EHeapType HeapType, typename DataType>
	UINT64 HeapArray<HeapType, DataType>::MemSize()
	{
		return m_HeapSize;
	}
	template<EHeapType HeapType, typename DataType>
	CD3DX12_GPU_DESCRIPTOR_HANDLE HeapArray<HeapType, DataType>::GPUHandle(DataType * Data)
	{
		return GetGPUHandle(Data->ID());
	}
	template<EHeapType HeapType, typename DataType>
	CD3DX12_CPU_DESCRIPTOR_HANDLE HeapArray<HeapType, DataType>::CPUHandle(DataType * Data)
	{
		return GetCPUHandle(Data->ID());
	}
	template<EHeapType HeapType, typename DataType>
	ID3D12DescriptorHeap * HeapArray<HeapType, DataType>::GetHeap()
	{
		return m_Heap.Get();
	}
	template<EHeapType HeapType, typename DataType>
	D3D12_DESCRIPTOR_HEAP_DESC HeapArray<HeapType, DataType>::GetDesc(UINT Size)
	{
		D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
		Desc.NumDescriptors = m_HeapSize;
		Desc.NodeMask = 0;
		switch (HeapType)
		{
		case EHeapType::CBV:
		case EHeapType::SRV:
		case EHeapType::UAV:
			Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			break;
		case EHeapType::RTV:
			Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			break;
		case EHeapType::DSV:
			Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			break;
		}
		return Desc;
	}

	template<EHeapType HeapType, typename DataType>
	CD3DX12_GPU_DESCRIPTOR_HANDLE HeapArray<HeapType, DataType>::GetGPUHandle(UINT Index)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE Handle(m_Heap->GetGPUDescriptorHandleForHeapStart());
		switch (HeapType)
		{
		case EHeapType::CBV:
		case EHeapType::SRV:
		case EHeapType::UAV:
			Handle.Offset(Index, m_Device->CbvSrvUavDescriptorSize());
			break;
		case EHeapType::RTV:
			Handle.Offset(Index, m_Device->RtvDescriptorSize());
			break;
		case EHeapType::DSV:
			Handle.Offset(Index, m_Device->DsvDescriptorSize());
			break;
		}
		return Handle;
	}

	template<EHeapType HeapType, typename DataType>
	CD3DX12_CPU_DESCRIPTOR_HANDLE HeapArray<HeapType, DataType>::GetCPUHandle(UINT Index)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE Handle(m_Heap->GetCPUDescriptorHandleForHeapStart());
		switch (HeapType)
		{
		case EHeapType::CBV:
		case EHeapType::SRV:
		case EHeapType::UAV:
			Handle.Offset(Index, m_Device->CbvSrvUavDescriptorSize());
			break;
		case EHeapType::RTV:
			Handle.Offset(Index, m_Device->RtvDescriptorSize());
			break;
		case EHeapType::DSV:
			Handle.Offset(Index, m_Device->DsvDescriptorSize());
			break;
		}
		return Handle;
	}
}

