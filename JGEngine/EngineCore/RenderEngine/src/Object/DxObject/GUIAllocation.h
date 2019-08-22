#pragma once

#include "RE.h"
#include "Resource.h"
#include <d3d12.h>
#include <iostream>

namespace RE
{
	class RenderDevice;
	class RENDERENGINE_API GUIAllocation
	{
		friend RenderDevice;
	private:
	
		D3D12_CPU_DESCRIPTOR_HANDLE m_Cpu;
		D3D12_GPU_DESCRIPTOR_HANDLE m_Gpu;
		std::shared_ptr<D3D12_SHADER_RESOURCE_VIEW_DESC> m_SrvDesc;
		std::string m_ManagedResourceName;
	public:
		GUIAllocation() {}
		GUIAllocation(
			D3D12_CPU_DESCRIPTOR_HANDLE cpu, 
			D3D12_GPU_DESCRIPTOR_HANDLE gpu,
			const std::string& resourceName,
			D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) :
			m_Cpu(cpu), m_Gpu(gpu), m_ManagedResourceName(resourceName)
		{
			if (srvDesc)
				m_SrvDesc = std::make_shared<D3D12_SHADER_RESOURCE_VIEW_DESC>(*srvDesc);		
		}
		uint64_t Get() const {
			return m_Gpu.ptr;
		}
	};
}