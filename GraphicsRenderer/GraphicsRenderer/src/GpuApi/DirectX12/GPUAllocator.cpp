#include"pch.h"

#include"GPUAllocator.h"
#include"Dx12Helper.h"

using namespace std;


namespace GR
{
	namespace Dx12
	{

		GPUAllocation::GPUAllocation()
		{
			m_Index = -1;
			m_Owner = nullptr;
			m_CPU.ptr = 0;
			m_GPU.ptr = 0;
		}
		GPUAllocation::~GPUAllocation()
		{
			if (m_Owner)
			{
				m_Owner->Free(this);
			}

			m_CPU.ptr = 0;
			m_GPU.ptr = 0;
			m_Owner = nullptr;
		}

		GPUAllocation::GPUAllocation(GPUAllocation&& rhs)
		{
			m_Owner = rhs.m_Owner;
			m_Index = rhs.m_Index;
			m_CPU = rhs.m_CPU;
			m_GPU = rhs.m_GPU;


			rhs.m_Owner = nullptr;
			rhs.m_CPU.ptr = 0;
			rhs.m_GPU.ptr = 0;
			rhs.m_Index = -1;
		}
		GPUAllocation& GPUAllocation::operator=(GPUAllocation&& rhs)
		{
			m_Owner = rhs.m_Owner;
			m_Index = rhs.m_Index;
			m_CPU = rhs.m_CPU;
			m_GPU = rhs.m_GPU;


			rhs.m_Owner = nullptr;
			rhs.m_CPU.ptr = 0;
			rhs.m_GPU.ptr = 0;
			rhs.m_Index = -1;

			return *this;
		}
		GPUAllocator::GPUAllocator()
		{

		}

		void GPUAllocator::Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptor)
		{
			m_D3DDevice = device;
			m_GPUHeap = CreateD3DDescriptorHeap(device, type, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, numDescriptor);

			m_StartCPUHeapHandle = m_GPUHeap->GetCPUDescriptorHandleForHeapStart();
			m_StartGPUHeapHandle = m_GPUHeap->GetGPUDescriptorHandleForHeapStart();
			m_IncrementDescritporSize = device->GetDescriptorHandleIncrementSize(type);
			m_D3D_DescirptorHeapType = type;
			m_Offset = 0;
			m_NumDescriptor = numDescriptor;
		}

		GPUAllocation GPUAllocator::Allocate()
		{
			int offset = 0;
			if (m_PendingGPUOffset.empty())
			{
				offset = m_Offset++;
			}
			else
			{
				offset = m_PendingGPUOffset.front();
				m_PendingGPUOffset.pop();
			}

			if (m_Offset >= m_NumDescriptor)
			{
				assert(false && "GPUAllocator's Heap Size over");
			}
			GPUAllocation gpu;
			gpu.m_Index = offset;
			gpu.m_Owner = this;
			gpu.m_GPU.ptr = m_StartGPUHeapHandle.ptr + (m_IncrementDescritporSize * offset);
			gpu.m_CPU.ptr = m_StartCPUHeapHandle.ptr + (m_IncrementDescritporSize * offset);

			return gpu;
		}
		void GPUAllocator::Free(GPUAllocation* allocation)
		{
			int offset = allocation->m_Index;
			m_PendingGPUOffset.push(offset);
		}

	}
}