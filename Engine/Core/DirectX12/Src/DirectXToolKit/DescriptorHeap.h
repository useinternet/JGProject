#pragma once
#include"DirectXCommon/DirectXCommon.h"


namespace Dx12
{
	class DescriptorHeap
	{
		ComPtr<ID3D12DescriptorHeap> m_D3D_DescriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE  m_D3D_Type;
		D3D12_DESCRIPTOR_HEAP_FLAGS m_D3D_Flag;
		uint32_t m_IncrementDescriptorHeapSize;
	public:
		DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptor, D3D12_DESCRIPTOR_HEAP_FLAGS flag = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		~DescriptorHeap() = default;
	public:
		void Resize(uint32_t numDescriptor);

		D3D12_CPU_DESCRIPTOR_HANDLE GetStartCPUHeapHandle() const {
			return m_D3D_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		}
		D3D12_GPU_DESCRIPTOR_HANDLE GetStartGPUHeapHandle() const {
			return m_D3D_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHeapHandle(uint32_t offset) const {
			auto address = GetStartCPUHeapHandle();
			address.ptr += (offset * m_IncrementDescriptorHeapSize);
			return address;
		}
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHeapHandle(uint32_t offset) const {
			auto address = GetStartGPUHeapHandle();
			address.ptr += (offset * m_IncrementDescriptorHeapSize);
			return address;
		}
	public:
		ID3D12DescriptorHeap* Get() const {
			return m_D3D_DescriptorHeap.Get();
		}
		ComPtr<ID3D12DescriptorHeap> GetComPtr() const {
			return m_D3D_DescriptorHeap;
		}
	};
}