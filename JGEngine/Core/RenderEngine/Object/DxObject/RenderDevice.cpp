#include "pch.h"
#include "RenderDevice.h"
#include "DxUtill.h"
#include "Defines/Dx12Hash.h"
#include "DescriptorAllocator.h"
#include "DescriptorHandle.h"
#include "CommandQueue.h"
#include "Resource.h"
#include "CommandList.h"
#include "GUI.h"
#include "Shader.h"

using namespace std;
using namespace concurrency;
namespace RE
{
	RenderDevice::RenderDevice(const RenderDeviceDesc& desc) :
		DxObject("RenderDevice"),
		m_Frame(0),
		m_ValueIndex(0),
		m_RenderDeviceDesc(desc)
	{
		m_Factory = CreateDXGIFactory();
		m_Device = CreateD3DDevice(m_Factory, false);
		ReObject::m_RenderDevice = this;
	

		m_DirectCommandQueue  = make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_ComputeCommandQueue = make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		m_CopyCommandQueue    = make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY);
		m_FrameResources.resize(desc.bufferCount);


		m_SwapChain = CreateDXGISwapChain(desc.hWnd, m_Factory, m_DirectCommandQueue->GetD3DCommandQueue(),
			desc.format, desc.width, desc.height, desc.bufferCount);




		m_SrvAllocator = make_shared<DescriptorAllocator>(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_UavAllocator = make_shared<DescriptorAllocator>(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_CbvAllocator = make_shared<DescriptorAllocator>(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_RtvAllocator = make_shared<DescriptorAllocator>(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_DsvAllocator = make_shared<DescriptorAllocator>(
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV);


		m_BackBuffer.resize(desc.bufferCount);
		for (uint32_t i = 0; i < desc.bufferCount; ++i)
		{
			m_BackBuffer[i] = make_shared<Texture>();
			ComPtr<ID3D12Resource> d3d_resource;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(d3d_resource.GetAddressOf()));
			m_BackBuffer[i]->SetName("RenderDevice_BackBufferResource" + to_string(i));
			m_BackBuffer[i]->SetD3DResource(d3d_resource, D3D12_RESOURCE_STATE_COMMON);
		}
		m_FinalTexture = make_shared<Texture>();



		m_GUIHeapOffset = 0;
		m_GUIMaxNumDescriptor = desc.gui_numDescriptor;
		m_GUIHeapIncrementSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_GUIDescriptorHeap = CreateD3DDescriptorHeap(m_RenderDevice->GetD3DDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, m_GUIMaxNumDescriptor);

		if (desc.gui)
		{
			m_RenderDeviceDesc.gui->Init(this);
		}

	}
	RenderDevice::~RenderDevice()
	{
		if (m_RenderDeviceDesc.gui)
		{
			m_RenderDeviceDesc.gui->Destroy();
		}
		Flush();
	}
	void RenderDevice::Update()
	{
		m_RenderDevice->BeginFrame();


		auto render_task = make_task([&] {
			RenderUpdate();
		});
		auto compute_task = make_task([&] {
			ComputeUpdate();
		});
		auto copy_task = make_task([&]{
			CopyUpdate();
		});
		structured_task_group update_tasks;

		update_tasks.run(render_task);
		update_tasks.run(compute_task);
		update_tasks.run(copy_task);
		update_tasks.wait();

		m_RenderDevice->EndFrame();
	}
	
	void RenderDevice::SubmitToRender(uint32_t priority_number, const RDFrameSubmission& func)
	{
		m_RenderFrameSubmissionPool[priority_number].push_back(func);
	}
	void RenderDevice::SubmitToCompute(uint32_t priority_number,const RDFrameSubmission& func)
	{
		m_ComputeFrameSubmissionPool[priority_number].push_back(func);
	}
	void RenderDevice::SubmitToCopy(uint32_t priority_number,  const RDFrameSubmission& func)
	{
		m_CopyFrameSubmissionPool[priority_number].push_back(func);
	}
	void RenderDevice::SubmitFinalTexture(const Texture& texture)
	{
		if (m_FinalTexture->GetD3DResource() != texture.GetD3DResource())
		{
			*m_FinalTexture = texture;
		}
	}
	DescriptorHandle RenderDevice::SrvAllocate(uint32_t numDescriptor)
	{
		return move(m_SrvAllocator->Allocate(numDescriptor));
	}
	DescriptorHandle RenderDevice::UavAllocate(uint32_t numDescriptor)
	{
		return move(m_UavAllocator->Allocate(numDescriptor));
	}
	DescriptorHandle RenderDevice::CbvAllocate(uint32_t numDescriptor)
	{
		return move(m_CbvAllocator->Allocate(numDescriptor));
	}
	DescriptorHandle RenderDevice::RtvAllocate(uint32_t numDescriptor)
	{
		return move(m_RtvAllocator->Allocate(numDescriptor));
	}
	DescriptorHandle RenderDevice::DsvAllocate(uint32_t numDescriptor)
	{
		return move(m_DsvAllocator->Allocate(numDescriptor));
	}
	void RenderDevice::GUIAllocate(const std::string& managedresourceName, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
	{
		lock_guard<mutex> lock(m_GuiAllocationMutex);
		if (m_ManagedTextureMap.find(managedresourceName) == m_ManagedTextureMap.end())
		{
			RE_LOG_ERROR("{0} is not Resource Registered in RenderDevice", managedresourceName);
				return;
		}
		size_t seed = std::hash<std::string>{}(managedresourceName);
		size_t desc_seed = 0;
		if (srvDesc)
		{
			desc_seed = std::hash<D3D12_SHADER_RESOURCE_VIEW_DESC>{}(*srvDesc);
		}
		std::hash_combine(seed, desc_seed);

		if (m_GUIAllocationMap.find(seed) != m_GUIAllocationMap.end())
		{
			return;
		}

		// 보류중인 gui 메모리가 없다면..
		if (m_PendingGUIAllocations.empty())
		{
			// gui힙에 할당할 메모리가 없을경우
			if (m_GUIHeapOffset >= m_GUIMaxNumDescriptor)
			{
				// 크기를 2배로 늘리고 다시 할당한다.
				m_GUIHeapOffset = 0;
				m_GUIMaxNumDescriptor *= 2;
				while (!m_PendingGUIAllocations.empty())
				{
					m_PendingGUIAllocations.pop();
				}
				m_GUIDescriptorHeap.Reset();
				m_GUIDescriptorHeap = CreateD3DDescriptorHeap(m_RenderDevice->GetD3DDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
					D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, m_GUIMaxNumDescriptor);


				for (auto& gui_allocation : m_GUIAllocationMap)
				{
					uint64_t offset = m_GUIHeapOffset * m_GUIHeapIncrementSize;
					gui_allocation.second.m_Cpu = { m_GUIDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + offset };
					gui_allocation.second.m_Gpu = { m_GUIDescriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr + offset };
					++m_GUIHeapOffset;
					m_Device->CreateShaderResourceView(
						m_ManagedTextureMap[gui_allocation.second.m_ManagedResourceName].GetD3DResource(),
						gui_allocation.second.m_SrvDesc.get(),
						gui_allocation.second.m_Cpu);
				}

				GUIAllocatorReAllocatedNoticeEvent e;
				e.ID = RenderEngineID::RenderDevice;
				GlobalLinkData::OnEvent(e);
			}
			// 할당
			auto cpu_start = m_GUIDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			auto gpu_start = m_GUIDescriptorHeap->GetGPUDescriptorHandleForHeapStart();


			uint64_t offset = m_GUIHeapOffset * m_GUIHeapIncrementSize;
			D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { cpu_start.ptr + offset };
			D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = { gpu_start.ptr + offset };
			++m_GUIHeapOffset;

			m_Device->CreateShaderResourceView(
				m_ManagedTextureMap[managedresourceName].GetD3DResource(), srvDesc, cpu_handle);

			GUIAllocation alloc(cpu_handle, gpu_handle, managedresourceName, srvDesc);
			m_GUIAllocationMap.emplace(seed, move(alloc));
		}
		else
		{
			auto gui_cpu = m_PendingGUIAllocations.front().m_Cpu;
			auto gui_gpu = m_PendingGUIAllocations.front().m_Gpu;
			m_PendingGUIAllocations.pop();

			m_Device->CreateShaderResourceView(
				m_ManagedTextureMap[managedresourceName].GetD3DResource(), srvDesc, gui_cpu);


			GUIAllocation alloc(gui_cpu, gui_gpu, managedresourceName, srvDesc);
			m_GUIAllocationMap.emplace(seed, move(alloc));
		}
	}
	void RenderDevice::GUIUnAllocate(const std::string& managedresourceName, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
	{
		lock_guard<mutex> lock(m_GuiAllocationMutex);
		if (m_ManagedTextureMap.find(managedresourceName) == m_ManagedTextureMap.end())
		{
			RE_LOG_ERROR("{0} is not Resource Registered in RenderDevice", managedresourceName);
			return;
		}
		size_t seed = std::hash<std::string>{}(managedresourceName);
		size_t desc_seed = 0;
		if (srvDesc)
		{
			desc_seed = std::hash<D3D12_SHADER_RESOURCE_VIEW_DESC>{}(*srvDesc);
		}
		std::hash_combine(seed, desc_seed);
		if (m_GUIAllocationMap.find(seed) == m_GUIAllocationMap.end())
		{
			RE_LOG_WARN("{0}'s ID is not exsit in GUIAllocationMap", managedresourceName);
			return;
		}
		m_PendingGUIAllocations.push(m_GUIAllocationMap[seed]);
		m_GUIAllocationMap.erase(seed);
	}
	uint64_t RenderDevice::GetGUIAllocation(const std::string& managedresourceName, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
	{
		lock_guard<mutex> lock(m_GuiAllocationMutex);
		if (m_ManagedTextureMap.find(managedresourceName) == m_ManagedTextureMap.end())
		{
			RE_LOG_ERROR("{0} is not Resource Registered in RenderDevice", managedresourceName);
			return 0;
		}
		size_t seed = std::hash<std::string>{}(managedresourceName);
		size_t desc_seed = 0;
		if (srvDesc)
		{
			desc_seed = std::hash<D3D12_SHADER_RESOURCE_VIEW_DESC>{}(*srvDesc);
		}
		std::hash_combine(seed, desc_seed);
		if (m_GUIAllocationMap.find(seed) == m_GUIAllocationMap.end())
		{
			RE_LOG_ERROR("{0}'s ID is not exsit in GUIAllocationMap", managedresourceName);
			return 0;
		}
		return m_GUIAllocationMap[seed].Get();
	}
	void RenderDevice::RegisterGUITexture(const Texture& resource)
	{
		lock_guard<mutex> lock(m_GuiAllocationMutex);
		if (resource.IsVaild())
		{
			if (m_ManagedTextureMap.find(resource.GetName()) != m_ManagedTextureMap.end())
			{
				RE_LOG_ERROR("{0} already register in RenderDevice, So Previously registered textures are ignored.", resource.GetName());
			}
			m_ManagedTextureMap[resource.GetName()] = resource;
		}
	}
	const Resource& RenderDevice::GetRegisteredGUITexture(const std::string& managedresourceName)
	{
		lock_guard<mutex> lock(m_GuiAllocationMutex);
		if (m_ManagedTextureMap.find(managedresourceName) == m_ManagedTextureMap.end())
		{
			RE_LOG_ERROR("{0} is not Resource Registered in RenderDevice", managedresourceName);
			m_ManagedTextureMap["None"];
		}
		return m_ManagedTextureMap[managedresourceName];
	}
	bool RenderDevice::CompileShader(Shader& shader, const ShaderCompiler& compiler)
	{
		if (shader.ToString() == "None")
		{
			return false;
		}


		ComPtr<ID3DBlob> errorCode;


		HRESULT hr = D3DCompile2(
			shader.m_ShaderCode.data(),
			shader.m_ShaderCode.size(),
			compiler.SourceName.data(),
			compiler.Macros.data(),
			compiler.pIncludes,
			compiler.EntryPoint.c_str(),
			compiler.CompileVersion.c_str(),
			compiler.Flags1,
			compiler.Flags2,
			compiler.SecondaryDataFlags,
			compiler.SecondaryData,
			compiler.SecondaryDataSize,
			shader.m_CompiledShaderCode.GetAddressOf(),
			errorCode.GetAddressOf());

		if(errorCode)
			shader.m_ErrorCode = reinterpret_cast<char*>(errorCode->GetBufferPointer());

		if (FAILED(hr))
			return false;

		return true;
	}
	void RenderDevice::Resize(uint32_t width, uint32_t height)
	{
		Flush();

		m_RenderDeviceDesc.width = width;
		m_RenderDeviceDesc.height = height;

		for (auto& resource : m_BackBuffer)
		{
			resource->Reset();
		}
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		m_SwapChain->GetDesc(&swapChainDesc);
		HRESULT hr = m_SwapChain->ResizeBuffers(
			swapChainDesc.BufferCount,
			width, height,
			swapChainDesc.BufferDesc.Format, 
			swapChainDesc.Flags);


		for (uint32_t i = 0; i < m_RenderDeviceDesc.bufferCount; ++i)
		{
			ComPtr<ID3D12Resource> d3d_resource;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(d3d_resource.GetAddressOf()));
			m_BackBuffer[i]->SetName("RenderDevice_BackBufferResource" + to_string(i));
			m_BackBuffer[i]->SetD3DResource(d3d_resource, D3D12_RESOURCE_STATE_COMMON);
		}
		m_ValueIndex = m_SwapChain->GetCurrentBackBufferIndex();
	}
	void RenderDevice::GetGUIAllocatorDebugInfo(Debug::GUIAllocatorInfo& out_debug_info)
	{
		for (auto& gui_pair : m_GUIAllocationMap)
		{
			auto& gui_allocation = gui_pair.second;

			Debug::GUIAllocationInfo info;
			info.cpu_address = gui_allocation.m_Cpu.ptr;
			info.gpu_address = gui_allocation.m_Gpu.ptr;
			info.bind_reosurce_name = gui_allocation.m_ManagedResourceName;
			out_debug_info.AllocationInfo.push_back(info);
		}

		out_debug_info.numDescriptor = m_GUIMaxNumDescriptor;
		out_debug_info.num_allocated_descriptor = m_GUIHeapOffset;

	}
	void RenderDevice::GetSrvDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info)
	{
		m_SrvAllocator->GetDebugInfo(out_debug_info);
		out_debug_info.DescriptorAllocatorType = "ShaderResourceView";
	}
	void RenderDevice::GetUavDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info)
	{
		m_UavAllocator->GetDebugInfo(out_debug_info);
		out_debug_info.DescriptorAllocatorType = "UnorderedAccessView";
	}
	void RenderDevice::GetCbvDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info)
	{
		m_CbvAllocator->GetDebugInfo(out_debug_info);
		out_debug_info.DescriptorAllocatorType = "ConstantBufferView";
	}
	void RenderDevice::GetRtvDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info)
	{
		m_RtvAllocator->GetDebugInfo(out_debug_info);
		out_debug_info.DescriptorAllocatorType = "RenderTargetView";
	}
	void RenderDevice::GetDsvDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info)
	{
		m_DsvAllocator->GetDebugInfo(out_debug_info);
		out_debug_info.DescriptorAllocatorType = "DepthStencilView";
	}
	ID3D12Device* RenderDevice::GetD3DDevice()
	{
		return m_Device.Get();
	}
	IDXGIFactory4* RenderDevice::GetDxgiFactory()
	{
		return m_Factory.Get();
	}
	uint64_t RenderDevice::GetFrameCount() const
	{
		return m_FrameResources[m_ValueIndex].EngineFrameCount;
	}
	uint32_t RenderDevice::GetValueIndex() const
	{
		return m_ValueIndex;
	}
	const RenderDeviceDesc& RenderDevice::GetDesc() const
	{
		return m_RenderDeviceDesc;
	}
	void RenderDevice::BeginFrame()
	{
		++m_Frame;
	}
	void RenderDevice::RenderUpdate()
	{
		auto& frameresource = m_FrameResources[m_ValueIndex];
		m_DirectCommandQueue->WaitForFenceValue(frameresource.DirectFenceValue);


		std::vector<CommandList*> cmdList_vector;



		for (auto& frame_submissons : m_RenderFrameSubmissionPool)
		{

			uint32_t submisson_count = (uint32_t)frame_submissons.second.size();
			std::vector<CommandList*> temp_cmdList_array(submisson_count);

			parallel_for((uint32_t)0, submisson_count, [&](uint32_t index)
			{
				temp_cmdList_array[index] = m_DirectCommandQueue->GetCommandList();
				frame_submissons.second[index](temp_cmdList_array[index]);
			});

			cmdList_vector.insert(
				cmdList_vector.end(),
				temp_cmdList_array.begin(), temp_cmdList_array.end());
		}



		auto present_commandList = m_DirectCommandQueue->GetCommandList();
		Present(present_commandList, *m_FinalTexture);
		cmdList_vector.push_back(present_commandList);
		


		m_DirectCommandQueue->ExcuteCommandList(cmdList_vector);
		m_SwapChain->Present(false, 0);
		frameresource.DirectFenceValue = m_DirectCommandQueue->Signal();
		m_DirectCommandQueue->Flush();
	}
	void RenderDevice::ComputeUpdate()
	{
		auto& frameresource = m_FrameResources[m_ValueIndex];
		m_ComputeCommandQueue->WaitForFenceValue(frameresource.ComputeFenceValue);



		std::vector<CommandList*> cmdList_vector;

		for (auto& frame_submissons : m_ComputeFrameSubmissionPool)
		{

			uint32_t submisson_count = (uint32_t)frame_submissons.second.size();
			std::vector<CommandList*> temp_cmdList_array(submisson_count);

			parallel_for((uint32_t)0, submisson_count, [&](uint32_t index)
			{
				temp_cmdList_array[index] = m_DirectCommandQueue->GetCommandList();
				frame_submissons.second[index](temp_cmdList_array[index]);
			});

			cmdList_vector.insert(
				cmdList_vector.end(),
				temp_cmdList_array.begin(), temp_cmdList_array.end());
		}


		m_ComputeCommandQueue->ExcuteCommandList(cmdList_vector);
		frameresource.ComputeFenceValue = m_ComputeCommandQueue->Signal();

	}
	void RenderDevice::CopyUpdate()
	{
		auto& frameresource = m_FrameResources[m_ValueIndex];
		m_CopyCommandQueue->WaitForFenceValue(frameresource.CopyFenceValue);


		std::vector<CommandList*> cmdList_vector;

		for (auto& frame_submissons : m_CopyFrameSubmissionPool)
		{

			uint32_t submisson_count = (uint32_t)frame_submissons.second.size();
			std::vector<CommandList*> temp_cmdList_array(submisson_count);

			parallel_for((uint32_t)0, submisson_count, [&](uint32_t index)
			{
				temp_cmdList_array[index] = m_DirectCommandQueue->GetCommandList();
				frame_submissons.second[index](temp_cmdList_array[index]);
			});

			cmdList_vector.insert(
				cmdList_vector.end(),
				temp_cmdList_array.begin(), temp_cmdList_array.end());
		}


		m_CopyCommandQueue->ExcuteCommandList(cmdList_vector);
		frameresource.CopyFenceValue = m_CopyCommandQueue->Signal();
	}
	void RenderDevice::EndFrame()
	{
		m_ValueIndex++;
		m_ValueIndex = m_ValueIndex % m_RenderDeviceDesc.bufferCount;
		m_FrameResources[m_ValueIndex].EngineFrameCount = m_Frame;

		m_SrvAllocator->ReleaseFreeReserveredDescriptorHandles();
		m_UavAllocator->ReleaseFreeReserveredDescriptorHandles();
		m_CbvAllocator->ReleaseFreeReserveredDescriptorHandles();
		m_RtvAllocator->ReleaseFreeReserveredDescriptorHandles();
		m_DsvAllocator->ReleaseFreeReserveredDescriptorHandles();

		m_RenderFrameSubmissionPool.clear();
		m_ComputeFrameSubmissionPool.clear();
		m_CopyFrameSubmissionPool.clear();
	}
	void RenderDevice::Present(CommandList* commandList, const Texture& texture)
	{
		auto& resource = m_BackBuffer[m_ValueIndex];
		auto rtv = resource->GetRtv();

		commandList->ClearRenderTarget(*resource, Color(0.0f,0.0f,0.0f,0.0f));
		

		if (texture.IsVaild())
		{
			commandList->CopyResource(*resource, texture);
		}
		

		commandList->SetRenderTarget(*resource);
		commandList->FlushResourceBarrier();


		commandList->GetD3DCommandList()->SetDescriptorHeaps(1, m_GUIDescriptorHeap.GetAddressOf());
		if (m_RenderDeviceDesc.gui)
		{
			m_RenderDeviceDesc.gui->Render(commandList->GetD3DCommandList());
		}
		commandList->TransitionBarrier(*resource, D3D12_RESOURCE_STATE_PRESENT);

		commandList->DeleteTemporaryResource(*resource);
	}
	void RenderDevice::Flush()
	{
		m_DirectCommandQueue->Flush();
		m_ComputeCommandQueue->Flush();
		m_CopyCommandQueue->Flush();
	}
}