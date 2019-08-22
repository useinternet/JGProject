#pragma once
#include"Dx12Include.h"

namespace GR
{
	namespace Dx12
	{
		class GPUAllocator;
		class GPUAllocation
		{
			friend GPUAllocator;
			GPUAllocator* m_Owner;
			uint32_t m_Index;
			D3D12_CPU_DESCRIPTOR_HANDLE m_CPU;
			D3D12_GPU_DESCRIPTOR_HANDLE m_GPU;
		private:
			GPUAllocation(const GPUAllocation& copy) = delete;
			GPUAllocation& operator=(const GPUAllocation& rhs) = delete;
		public:
			GPUAllocation();
			~GPUAllocation();

			GPUAllocation(GPUAllocation&& rhs);
			GPUAllocation& operator=(GPUAllocation&& rhs);


			D3D12_GPU_DESCRIPTOR_HANDLE GPU() const {
				return m_GPU;
			}
			D3D12_CPU_DESCRIPTOR_HANDLE CPU() const {
				return m_CPU;
			}
			bool IsNull() const {
				return (m_GPU.ptr == 0 || m_CPU.ptr == 0);
			}
		};
		class GPUAllocator
		{
			friend GPUAllocation;
		private:
			ID3D12Device* m_D3DDevice;
			ComPtr<ID3D12DescriptorHeap>  m_GPUHeap;
			D3D12_CPU_DESCRIPTOR_HANDLE   m_StartCPUHeapHandle;
			D3D12_GPU_DESCRIPTOR_HANDLE   m_StartGPUHeapHandle;
			uint32_t                      m_IncrementDescritporSize;
			D3D12_DESCRIPTOR_HEAP_TYPE    m_D3D_DescirptorHeapType;
			uint32_t                      m_Offset;
			uint32_t                      m_NumDescriptor;
			std::queue<uint32_t>          m_PendingGPUOffset;
			std::mutex m_AllcateMutex;
		public:
			GPUAllocator();
			void Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptor);

			GPUAllocation Allocate();
		

			ComPtr<ID3D12DescriptorHeap> GetD3DDescriptorHeap() const {
				return m_GPUHeap;
			}
		private:
			void Free(GPUAllocation* allocation);
		};
	}
}