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
#include "DxScreen.h"


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
		m_FrameResources.resize(csm_Engine_BufferCount);

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
		ENGINE_PERFORMANCE_TIMER("RenderEngine", "RenderDevice::Update()");
		m_RenderDevice->BeginFrame();
		std::vector<CommandList*> render_List;
		std::vector<CommandList*> compute_List;
		std::vector<CommandList*> copy_List;

		auto render_task = make_task([&] {
			render_List = RenderUpdate();
		});
		auto compute_task = make_task([&] {
			compute_List = ComputeUpdate();
		});
		auto copy_task = make_task([&]{
			copy_List = CopyUpdate();
		});
		structured_task_group update_tasks;

		update_tasks.run(render_task);
		update_tasks.run(compute_task);
		update_tasks.run(copy_task);
		update_tasks.wait();


		// FrameResource
		auto& frameresource = m_FrameResources[m_ValueIndex];
		// Direct
		m_DirectCommandQueue->ExcuteCommandList(render_List);
		for (auto& screen : m_DxScreenPool)
		{
			screen.second->Present();
		}
		frameresource.DirectFenceValue = m_DirectCommandQueue->Signal();
		//m_DirectCommandQueue->Flush();

		// Compute
		m_ComputeCommandQueue->ExcuteCommandList(compute_List);
		frameresource.ComputeFenceValue = m_ComputeCommandQueue->Signal();

		//Copy
		m_CopyCommandQueue->ExcuteCommandList(copy_List);
		frameresource.CopyFenceValue = m_CopyCommandQueue->Signal();

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
	//void RenderDevice::GetSrvDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info)
	//{
	//	m_SrvAllocator->GetDebugInfo(out_debug_info);
	//	out_debug_info.DescriptorAllocatorType = "ShaderResourceView";
	//}
	//void RenderDevice::GetUavDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info)
	//{
	//	m_UavAllocator->GetDebugInfo(out_debug_info);
	//	out_debug_info.DescriptorAllocatorType = "UnorderedAccessView";
	//}
	//void RenderDevice::GetCbvDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info)
	//{
	//	m_CbvAllocator->GetDebugInfo(out_debug_info);
	//	out_debug_info.DescriptorAllocatorType = "ConstantBufferView";
	//}
	//void RenderDevice::GetRtvDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info)
	//{
	//	m_RtvAllocator->GetDebugInfo(out_debug_info);
	//	out_debug_info.DescriptorAllocatorType = "RenderTargetView";
	//}
	//void RenderDevice::GetDsvDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info)
	//{
	//	m_DsvAllocator->GetDebugInfo(out_debug_info);
	//	out_debug_info.DescriptorAllocatorType = "DepthStencilView";
	//}
	DxScreen* RenderDevice::CreateDxScreen(HWND hWnd, uint32_t width, uint32_t height, DXGI_FORMAT format)
	{
		if (m_DxScreenMapByHWND.find(hWnd) != m_DxScreenMapByHWND.end())
			return nullptr;

		auto screen = make_shared<DxScreen>();
		DxScreenDesc desc;
		desc.bufferCount = csm_Engine_BufferCount;
		desc.width = width;
		desc.height = height;
		desc.format = format;
		desc.hWnd = hWnd;
		screen->Init(desc);

		auto p = screen.get();
		m_DxScreenPool[p] = screen;
		m_DxScreenMapByHWND[hWnd] = p;

		return p;
	}
	void RenderDevice::DestroyDxScreen(HWND hWnd)
	{
		if (m_DxScreenMapByHWND.find(hWnd) == m_DxScreenMapByHWND.end())
			return;
		auto p = m_DxScreenMapByHWND[hWnd];
		m_DxScreenMapByHWND.erase(hWnd);
		m_DxScreenPool.erase(p);
	}
	void RenderDevice::DestroyDxScreen(DxScreen* screen)
	{
		if (m_DxScreenPool.find(screen) == m_DxScreenPool.end())
			return;
		auto p = m_DxScreenMapByHWND[screen->GetDesc().hWnd];
		m_DxScreenMapByHWND.erase(screen->GetDesc().hWnd);
		m_DxScreenPool.erase(p);
	}
	DxScreen* RenderDevice::FindDxScreen(HWND hWnd)
	{
		if (m_DxScreenMapByHWND.find(hWnd) == m_DxScreenMapByHWND.end())
			return nullptr;
		return m_DxScreenMapByHWND[hWnd];
	}
	void RenderDevice::Flush()
	{
		m_DirectCommandQueue->Flush();
		m_ComputeCommandQueue->Flush();
		m_CopyCommandQueue->Flush();
	}
	ID3D12Device* RenderDevice::GetD3DDevice()
	{
		return m_Device.Get();
	}
	IDXGIFactory4* RenderDevice::GetDxgiFactory()
	{
		return m_Factory.Get();
	}
	CommandQueue* RenderDevice::GetDirectCmdQueue()
	{
		return m_DirectCommandQueue.get();
	}
	CommandQueue* RenderDevice::GetComputeCmdQueue()
	{
		return m_ComputeCommandQueue.get();
	}
	CommandQueue* RenderDevice::GetCopyCmdQueue()
	{
		return m_CopyCommandQueue.get();
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
	std::vector<CommandList*> RenderDevice::RenderUpdate()
	{
		ENGINE_PERFORMANCE_TIMER("RenderDevice::Update()", "RenderDevice::RenderUpdate()");
		auto& frameresource = m_FrameResources[m_ValueIndex];
		m_DirectCommandQueue->WaitForFenceValue(frameresource.DirectFenceValue);


		std::vector<CommandList*> cmdList_vector;


		// TestCode


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

		
		for (auto& screen : m_DxScreenPool)
		{
			auto present_commandList = m_DirectCommandQueue->GetCommandList();
			screen.second->Update(present_commandList);
			cmdList_vector.push_back(present_commandList);
		}
		

		return cmdList_vector;
	}
	std::vector<CommandList*> RenderDevice::ComputeUpdate()
	{
		ENGINE_PERFORMANCE_TIMER("RenderDevice::Update()", "RenderDevice::ComputeUpdate()");
		auto& frameresource = m_FrameResources[m_ValueIndex];
		m_ComputeCommandQueue->WaitForFenceValue(frameresource.ComputeFenceValue);



		std::vector<CommandList*> cmdList_vector;

		for (auto& frame_submissons : m_ComputeFrameSubmissionPool)
		{

			uint32_t submisson_count = (uint32_t)frame_submissons.second.size();
			std::vector<CommandList*> temp_cmdList_array(submisson_count);

			parallel_for((uint32_t)0, submisson_count, [&](uint32_t index)
			{
				temp_cmdList_array[index] = m_ComputeCommandQueue->GetCommandList();
				frame_submissons.second[index](temp_cmdList_array[index]);
			});

			cmdList_vector.insert(
				cmdList_vector.end(),
				temp_cmdList_array.begin(), temp_cmdList_array.end());
		}

		return cmdList_vector;
	}
	std::vector<CommandList*> RenderDevice::CopyUpdate()
	{
		ENGINE_PERFORMANCE_TIMER("RenderDevice::Update()", "RenderDevice::CopyUpdate()");
		auto& frameresource = m_FrameResources[m_ValueIndex];
		m_CopyCommandQueue->WaitForFenceValue(frameresource.CopyFenceValue);


		std::vector<CommandList*> cmdList_vector;

		for (auto& frame_submissons : m_CopyFrameSubmissionPool)
		{

			uint32_t submisson_count = (uint32_t)frame_submissons.second.size();
			std::vector<CommandList*> temp_cmdList_array(submisson_count);

			parallel_for((uint32_t)0, submisson_count, [&](uint32_t index)
			{
				temp_cmdList_array[index] = m_CopyCommandQueue->GetCommandList();
				frame_submissons.second[index](temp_cmdList_array[index]);
			});

			cmdList_vector.insert(
				cmdList_vector.end(),
				temp_cmdList_array.begin(), temp_cmdList_array.end());
		}

		return cmdList_vector;
	}
	void RenderDevice::EndFrame()
	{
		// °ª º¯°æ
		m_ValueIndex++;
		m_ValueIndex = m_ValueIndex % csm_Engine_BufferCount;
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

}