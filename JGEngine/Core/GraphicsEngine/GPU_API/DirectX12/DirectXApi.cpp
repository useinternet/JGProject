#include "pch.h"
#include "DirectXApi.h"
#include "DirectXTex/DirectXTex.h"
#include <dxgidebug.h>
#include <concurrent_queue.h>
#include <stack>
#include <tuple>
#include <shared_mutex>
using namespace DirectX;

using namespace std;
using namespace std::chrono;
using namespace Microsoft::WRL;
using namespace Concurrency;

#define DX12_SHADER_ENTRY   "main"
#define DX12_SHADER_VERSION "5_1"

// DEFINES
namespace DX12
{
	class ResourceRefInfo;
	class _DXCommandResource;
	class FrameResource;
	class Screen;
	class ResourceStateTracker;



	DescriptorAllocation CSUAllocate();
	DescriptorAllocation RTVAllocate();
	DescriptorAllocation DSVAllocate();
	void ResourceRefStateLock();
	void ResourceRefStateUnLock();
	void RegisterResource(Resource& src, const std::string& name, ID3D12Resource* d3d_resource, D3D12_RESOURCE_STATES initState);
	void IncreaseResourceRefCount(ID3D12Resource* d3d_resource);
	void DecreaseResourceRefCount(ID3D12Resource* d3d_resource);



	// D3D ï¿½ï¿½Ä¡
	ComPtr<IDXGIFactory4>   CreateDXGIFactory();
	ComPtr<ID3D12Device>    CreateD3DDevice(ComPtr<IDXGIFactory4> factory, bool is_UseWrapDevice, DXGI_ADAPTER_DESC1* OutadapterDesc);
	ComPtr<IDXGISwapChain4> CreateDXGISwapChain(
		HWND hWnd, ComPtr<IDXGIFactory4> factory, ComPtr<ID3D12CommandQueue> cmdQue, 
		DXGI_FORMAT format, uint32_t width, uint32_t height, uint32_t bufferCount);
	// D3D Command
	ComPtr<ID3D12CommandQueue>        CreateD3DCommandQueue(ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);
	ComPtr<ID3D12GraphicsCommandList> CreateD3DCommandList(ComPtr<ID3D12Device> device, ComPtr<ID3D12CommandAllocator> cmdAllocator, D3D12_COMMAND_LIST_TYPE type);
	ComPtr<ID3D12CommandAllocator> CreateD3DCommandAllocator(ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);


	// D3D Coms
	ComPtr<ID3D12DescriptorHeap> CreateD3DDescriptorHeap(ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag, uint32_t numDescriptor);
	ComPtr<ID3D12RootSignature>  CreateD3DRootSignature(ComPtr<ID3D12Device> device, CD3DX12_ROOT_SIGNATURE_DESC* DESC);







}
// Global Var
namespace DX12
{
	
	recursive_mutex g_ResourceMutex;
	mutex g_ResourceStateMutex;
	bool  g_ResourceLock = false;
	mutex g_CmdKeyMutex;
	shared_mutex g_LoadingTextureMutex;
	Microsoft::WRL::ComPtr<IDXGIFactory4> g_Factory;
	Microsoft::WRL::ComPtr<ID3D12Device>  g_Device;


	uint32_t g_BufferCount = 3;
	uint64_t g_BufferIndex = 0;

	shared_mutex g_ScreenMutex;
	unordered_map<HWND, unique_ptr<Screen>> g_ScreenPool;
	map<uint32_t, queue<ScreenInfo>> g_ResizingScreenQueue;

	std::unique_ptr<FrameResource> g_GraphicsFrameResource;
	std::unique_ptr<FrameResource> g_ComputeFrameResource;
	std::unique_ptr<FrameResource> g_CopyFrameResource;

	unique_ptr<DescriptorAllocator> g_CSUAllocator;
	unique_ptr<DescriptorAllocator> g_RTVAllocator;
	unique_ptr<DescriptorAllocator> g_DSVAllocator;


	// 
	std::unordered_map<ID3D12Resource*, ResourceRefInfo>    g_ResourceRefMap;
	std::unordered_map<string, pair<uint32_t, Texture>>     g_LoadingTextureCache;
	std::unordered_map<string, Texture>                     g_TextureCahce;
	std::unordered_map<size_t, ComPtr<ID3D12RootSignature>> g_RootSigCahce;
	std::unordered_map<size_t, ComPtr<ID3D12PipelineState>> g_GraphicsPSOCache;
	std::unordered_map<size_t, ComPtr<ID3D12PipelineState>> g_ComputePSOCache;
	std::vector<CommandKeyPtr> g_CommandKeyPool;
}
// Private in Class
namespace DX12
{
	class ResourceState
	{
	public:
		D3D12_RESOURCE_STATES Get(uint32_t subresource) const {
			if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) return State;
			else
			{
				auto iter = StateMap.find(subresource);
				if (iter == StateMap.end())
				{
					return StateMap.at(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
				}
				return iter->second;
			}

		}
		void Set(uint32_t subresource, D3D12_RESOURCE_STATES state)
		{
			if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
			{
				State = state;
				StateMap.clear();
			}
			else
			{
				StateMap[subresource] = state;

			}
		
		}
	public:
		map<uint32_t, D3D12_RESOURCE_STATES> StateMap;
		D3D12_RESOURCE_STATES State;
	};

	class ResourceRefInfo
	{
	public:
		ResourceState state;
		uint64_t refCount = 0;
		Resource rsc;
	};

	class ResourceStateTracker
	{
	public:
		void TransitionBarrier(const Resource& rsc, D3D12_RESOURCE_STATES state, uint32_t subResource);

		void UAVBarrier(const Resource& rsc);
		void AliasBarrier(const Resource& before_rsc, const Resource& after_rsc);

		void FlushResourceBarrier(ID3D12GraphicsCommandList* cmdList);
		bool FlushPendingResourceBarrier(ID3D12GraphicsCommandList* cmdList);
		void CommitResourceState();
		void Reset();
	private:
		vector<D3D12_RESOURCE_BARRIER> m_ResourceBarriers;
		vector<D3D12_RESOURCE_BARRIER> m_PendingResourceBarriers;
		unordered_map<ID3D12Resource*, ResourceState> m_ResourceStates;
	};

	class Fence
	{
	private:
		ComPtr<ID3D12Fence>  m_D3D_Fence;
		std::mutex           m_EventMutex;
		std::atomic_uint64_t m_FenceValue;
		HANDLE               m_FenceEvent;
	public:
		Fence();
		~Fence();
		// Fence ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
		uint64_t GetValue() const {
			return m_FenceValue;
		}
		// Fence ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
		void IncreaseValue() {
			m_FenceValue.fetch_add(1);
		}
		// Fence ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½È­ 
		void WaitForFenceValue(uint64_t value, std::chrono::milliseconds duration = std::chrono::milliseconds::max());

		// ID3D12Fence* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
		ID3D12Fence* Get() const {
			return m_D3D_Fence.Get();
		}
	};
	class _DXCommandResource
	{
	public:
		unique_ptr<UploadAllocator>              uploadAllocator;
		unique_ptr<DynamicDescriptorAllocator>   dynamicDescriptorAllocator;
		unique_ptr<ResourceStateTracker> resourceStateTracker;

		ComPtr<ID3D12PipelineState> bindedGraphicsPSO     = nullptr;
		ComPtr<ID3D12PipelineState> bindedComputePSO = nullptr;
		ComPtr<ID3D12RootSignature> bindedGraphicsRootSig = nullptr;
		ComPtr<ID3D12RootSignature> bindedComputeRootSig  = nullptr;

		ComPtr<ID3D12DescriptorHeap> bindedDescriptorHeap = nullptr;

		D3D12_COMMAND_LIST_TYPE           d3d_commandType;
		ComPtr<ID3D12GraphicsCommandList> d3d_commandList;
		ComPtr<ID3D12CommandAllocator>    d3d_Allocator;

		std::vector<Resource> tempResource;

		_DXCommandResource(D3D12_COMMAND_LIST_TYPE type)
		{
			d3d_commandType = type;
			d3d_Allocator = CreateD3DCommandAllocator(DXDevice::GetD3DDevice(), type);
			d3d_commandList = CreateD3DCommandList(DXDevice::GetD3DDevice(), d3d_Allocator.Get(), type);
			uploadAllocator = make_unique<UploadAllocator>();
			resourceStateTracker = make_unique<ResourceStateTracker>();
			dynamicDescriptorAllocator = make_unique<DynamicDescriptorAllocator>();
		}
		void BackupResource(const Resource& resource)
		{
			tempResource.push_back(resource);
		}
		void Close()
		{
			resourceStateTracker->FlushResourceBarrier(d3d_commandList.Get());
			d3d_commandList->Close();
		}
		bool Close(_DXCommandResource* pendingCmdResource)
		{
			Close();
			bool is_exist_barrier = resourceStateTracker->FlushPendingResourceBarrier(pendingCmdResource->d3d_commandList.Get());
			resourceStateTracker->CommitResourceState();
			return is_exist_barrier;
		}
		void Reset() {

		//	uploadAllocator->Reset();
			dynamicDescriptorAllocator->Reset();
			resourceStateTracker->Reset();
			uploadAllocator->Reset();
			tempResource.clear();

			d3d_Allocator->Reset();
			d3d_commandList->Reset(d3d_Allocator.Get(), nullptr);

			bindedDescriptorHeap = nullptr;
			bindedGraphicsPSO = nullptr;
			bindedComputePSO = nullptr;
			bindedGraphicsRootSig = nullptr;
			bindedComputeRootSig = nullptr;
		}
	};

	class FrameResource
	{
	private:
		_DXCommandResource* GetCommandResource()
		{
			unique_ptr<_DXCommandResource> cmdResource = nullptr;
			_DXCommandResource* result = nullptr;
			if (!abandonedCmdResources.empty())
			{
				cmdResource = move(abandonedCmdResources.front()); abandonedCmdResources.pop();
				cmdResource->Reset();
				result = cmdResource.get();
				cmdResources[cmdResource.get()] = move(cmdResource);
			}
			else
			{
				cmdResource = make_unique<_DXCommandResource>(d3d_type);
				result = cmdResource.get();
				cmdResources[cmdResource.get()] = move(cmdResource);
			}
			return result;
		}
	public:
		_DXCommandResource* GetExecuteCommandResource(int Priority = 0)
		{
			auto cmdResource = GetCommandResource();
			expect_execute_cmdList[Priority].push_back(cmdResource);
			return cmdResource;
		}
		
		void Flush()
		{
			fence->IncreaseValue();
			cmdQueue->Signal(fence->Get(), fence->GetValue());
			fence->WaitForFenceValue(fence->GetValue());
		}
		void BeginFrame()
		{
			uint64_t value = fenceValue[bufferIndex];
			fence->WaitForFenceValue(value);

			auto& cmdrscQue = pendingCmdResources[bufferIndex];
			while (!cmdrscQue.empty())
			{
				abandonedCmdResources.push(move(cmdrscQue.front()));
				cmdrscQue.pop();
			}

		}
		void EndFrame()
		{
		
			// commandList ï¿½ï¿½ï¿½ï¿½
			std::vector<ID3D12CommandList*>   d3d_cmdLists;
			ResourceRefStateLock();
			for (auto& cmdrscList : expect_execute_cmdList)
			{

				for (auto& cmdrsc : cmdrscList.second)
				{
					auto pendingCmdResource = GetCommandResource();

					bool has_pending_barrier = cmdrsc->Close(pendingCmdResource);
			
					pendingCmdResource->Close();
					if (has_pending_barrier)
					{
						d3d_cmdLists.push_back(pendingCmdResource->d3d_commandList.Get());
					}
					d3d_cmdLists.push_back(cmdrsc->d3d_commandList.Get());
	
					pendingCmdResources[bufferIndex].push(move(cmdResources[pendingCmdResource]));
					pendingCmdResources[bufferIndex].push(move(cmdResources[cmdrsc]));
					cmdResources.erase(pendingCmdResource);
					cmdResources.erase(cmdrsc);
				}
			}
			ResourceRefStateUnLock();
			expect_execute_cmdList.clear();
			cmdQueue->ExecuteCommandLists((uint32_t)d3d_cmdLists.size(), d3d_cmdLists.data());



			// fence  value Ã³ï¿½ï¿½
			fence->IncreaseValue();
			cmdQueue->Signal(fence->Get(), fence->GetValue());
			fenceValue[bufferIndex] = fence->GetValue();


		

			++bufferIndex;
			bufferIndex = bufferIndex % bufferCount;
		}
		ID3D12CommandQueue* GetD3DQueuePtr() const {
			return cmdQueue.Get();
		}
	public:
		FrameResource(uint64_t bufferCount, D3D12_COMMAND_LIST_TYPE type)
		{
			d3d_type = type;
			this->bufferCount = bufferCount;
			fence = make_unique<Fence>();
			cmdQueue = CreateD3DCommandQueue(DXDevice::GetD3DDevice(), d3d_type);
			fenceValue.resize(bufferCount);
		}
	private:
		ComPtr<ID3D12CommandQueue> cmdQueue;
		D3D12_COMMAND_LIST_TYPE    d3d_type;
		std::unordered_map<_DXCommandResource*, std::unique_ptr<_DXCommandResource>>   cmdResources;
		std::map<int, std::vector<_DXCommandResource*>> expect_execute_cmdList;
		std::map<uint64_t, std::queue<std::unique_ptr<_DXCommandResource>>>            pendingCmdResources;
		std::queue<std::unique_ptr<_DXCommandResource>> abandonedCmdResources;
		std::unique_ptr<Fence> fence;
		uint64_t bufferCount = 0;
		uint64_t bufferIndex = 0;
		vector<uint64_t> fenceValue;
	};

	class Screen
	{
	public:
		Screen(HWND hWnd, int width, int height, const  XMFLOAT4& clearColor) :
			m_hWnd(hWnd), m_Width(width), m_Height(height)
		{
			m_SwapChain = CreateDXGISwapChain(hWnd,
				DXDevice::GetDXGIFactory(),
				g_GraphicsFrameResource->GetD3DQueuePtr(),
				DXGI_FORMAT_R8G8B8A8_UNORM,
				m_Width, m_Height,
				g_BufferCount);

			for (int i = 0; i < (int)g_BufferCount; ++i)
			{
				m_RTV[i] = move(RTVAllocate());
				m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_BackBuffer[i].GetAddressOf()));
				g_Device->CreateRenderTargetView(m_BackBuffer[i].Get(), nullptr, m_RTV[i].CPU());
			}

			m_ClearColor[0] = clearColor.x;
			m_ClearColor[1] = clearColor.y;
			m_ClearColor[2] = clearColor.z;
			m_ClearColor[3] = clearColor.w;
		}
		void SubmitTexture(Texture& texture)
		{
			if (m_FinalTexture.GetD3DPtr() == texture.GetD3DPtr()) return;
			m_FinalTexture = texture;
		}
		void Resize(int width, int height)
		{
			if (m_Width == width && m_Height == height) return;
			for (int i = 0; i < (int)g_BufferCount; ++i)
			{
				m_BackBuffer[i].Reset();
			}

			DXGI_SWAP_CHAIN_DESC1 desc;
			m_SwapChain->GetDesc1(&desc);
			desc.Width  = max<int>(1, width);
			desc.Height = max<int>(1, height);
			m_Width = desc.Width;
			m_Height = desc.Height;
			m_SwapChain->ResizeBuffers(g_BufferCount, desc.Width, desc.Height, desc.Format, desc.Flags);

			for (int i = 0; i < (int)g_BufferCount; ++i)
			{
				m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_BackBuffer[i].GetAddressOf()));
				g_Device->CreateRenderTargetView(m_BackBuffer[i].Get(), nullptr, m_RTV[i].CPU());
			}
		}
		void Update()
		{
			uint32_t index = m_SwapChain->GetCurrentBackBufferIndex();
			auto& backBuffer = m_BackBuffer[index];


			CommandKeyPtr cmdKey = DXDevice::RequestGraphicsCommandKey();
			auto cmdList = cmdKey->cmdResource->d3d_commandList;


			cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));


			cmdList->ClearRenderTargetView(m_RTV[index].CPU(), m_ClearColor,
				0, nullptr);


			if (m_FinalTexture.IsVaild())
			{
				cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
					backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST));
				DXCommand::TransitionBarrier(cmdKey, m_FinalTexture, D3D12_RESOURCE_STATE_COPY_SOURCE);

				cmdList->CopyResource(backBuffer.Get(), m_FinalTexture.GetD3DPtr());


				cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
					backBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET));
			}


			cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	
		}
		void Present()
		{
			m_SwapChain->Present(0, 0);
		}
		ScreenInfo GetScreenInfo() const {
			ScreenInfo info;
			info.hWnd = m_hWnd; info.width = m_Width; info.height = m_Height;
			return info;
		}
	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain4>   m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D12Resource>    m_BackBuffer[3];
		DescriptorAllocation m_RTV[3];
		Texture m_FinalTexture;
		int  m_Width = 0, m_Height = 0;
		HWND m_hWnd;
		float m_ClearColor[4];
	};





}
// Private Function
namespace DX12
{
	DescriptorAllocation CSUAllocate()
	{
		return move(g_CSUAllocator->Allocate());
	}
	DescriptorAllocation RTVAllocate()
	{
		return move(g_RTVAllocator->Allocate());
	}
	DescriptorAllocation DSVAllocate()
	{
		return move(g_DSVAllocator->Allocate());
	}
	void ResourceRefStateLock()
	{
		if (g_ResourceLock) return;
		g_ResourceLock = true;
		g_ResourceMutex.lock();
	}
	void ResourceRefStateUnLock()
	{
		if (!g_ResourceLock) return;
		g_ResourceLock = false;
		g_ResourceMutex.unlock();
	}
	void RegisterResource(Resource& src, const std::string& name, ID3D12Resource* d3d_resource, D3D12_RESOURCE_STATES initState)
	{
		lock_guard<recursive_mutex> lock(g_ResourceMutex);
		auto& refInfo = g_ResourceRefMap[d3d_resource];
		refInfo.rsc.m_D3DResource = d3d_resource;
		refInfo.rsc.SetName(name);
		refInfo.state.Set(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, initState);
		refInfo.refCount = 1;
		src = refInfo.rsc;
	}
	void IncreaseResourceRefCount(ID3D12Resource* d3d_resource)
	{
		lock_guard<recursive_mutex> lock(g_ResourceMutex);
		if (d3d_resource == nullptr) return;
		if (g_ResourceRefMap.find(d3d_resource) == g_ResourceRefMap.end())
			assert("Resource::operator= : this resource must register Device");

		ResourceRefInfo& refInfo = g_ResourceRefMap[d3d_resource];

		refInfo.refCount++;
	}
	void DecreaseResourceRefCount(ID3D12Resource* d3d_resource)
	{
		lock_guard<recursive_mutex> lock(g_ResourceMutex);
		if (d3d_resource == nullptr) return;
		if (g_ResourceRefMap.find(d3d_resource) == g_ResourceRefMap.end())
			assert("Resource::operator= : this resource must register Device");


		ResourceRefInfo& refInfo = g_ResourceRefMap[d3d_resource];
		if (refInfo.refCount > 0) refInfo.refCount--;

		if (refInfo.refCount == 0)
		{
			g_ResourceRefMap.erase(d3d_resource);
		}
	}
}


// Device , DXCommand
namespace DX12
{
	void DXDevice::Create(int bufferCount)
	{
		g_Factory = CreateDXGIFactory();
		DXGI_ADAPTER_DESC1 output_desc;
		g_Device = CreateD3DDevice(g_Factory, false, &output_desc);
		// DescriptorAllocator ï¿½ï¿½ï¿½ï¿½
		g_CSUAllocator = make_unique<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		g_RTVAllocator = make_unique<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		g_DSVAllocator = make_unique<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		// FrameResource ï¿½ï¿½ï¿½ï¿½

		g_GraphicsFrameResource = make_unique<FrameResource>(g_BufferCount, D3D12_COMMAND_LIST_TYPE_DIRECT);
		g_ComputeFrameResource = make_unique<FrameResource>(g_BufferCount, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		g_CopyFrameResource = make_unique<FrameResource>(g_BufferCount, D3D12_COMMAND_LIST_TYPE_COPY);
	}

	void DXDevice::BeginFrame()
	{
		g_GraphicsFrameResource->BeginFrame();
		g_ComputeFrameResource->BeginFrame();
		g_CopyFrameResource->BeginFrame();


		for (auto iter = g_LoadingTextureCache.begin(); iter != g_LoadingTextureCache.end();)
		{
			if (iter->second.first == g_BufferIndex)
			{
				g_TextureCahce[iter->first] = move(iter->second.second);
				iter = g_LoadingTextureCache.erase(iter);
			}
			else ++iter;
		}
		

		while (!g_ResizingScreenQueue[GetBufferIndex()].empty())
		{
			auto info = g_ResizingScreenQueue[GetBufferIndex()].front();
			g_ResizingScreenQueue[GetBufferIndex()].pop();
			if (g_ScreenPool.find(info.hWnd) == g_ScreenPool.end()) continue;


			g_ScreenPool[info.hWnd]->Resize(info.width, info.height);
		}




	}
	void DXDevice::EndFrame()
	{

		for (auto& screen_pair : g_ScreenPool)
		{
			screen_pair.second->Update();
		}


		g_GraphicsFrameResource->EndFrame();
		g_ComputeFrameResource->EndFrame();
		g_CopyFrameResource->EndFrame();


		for (auto& screen_pair : g_ScreenPool)
		{
			screen_pair.second->Present();
		}


		g_BufferIndex++;
		g_BufferIndex = g_BufferIndex % g_BufferCount;
		g_CSUAllocator->UpdatePage();
		g_RTVAllocator->UpdatePage();
		g_DSVAllocator->UpdatePage();
	}

	void DXDevice::Destroy()
	{
		g_GraphicsFrameResource->Flush();
		g_ComputeFrameResource->Flush();
		g_CopyFrameResource->Flush();
		g_GraphicsFrameResource.reset();
		g_ComputeFrameResource.reset();
		g_CopyFrameResource.reset();
		g_CommandKeyPool.clear();


		g_ScreenPool.clear();
		g_LoadingTextureCache.clear();
		g_TextureCahce.clear();
		g_RootSigCahce.clear();
		g_GraphicsPSOCache.clear();
		g_ComputePSOCache.clear();


		for (auto& iter : g_ResourceRefMap)
		{
			iter.second.rsc.m_D3DResource = nullptr;

		}
		g_ResourceRefMap.clear();

		g_CSUAllocator.reset();
		g_RTVAllocator.reset();
		g_DSVAllocator.reset();


		if (g_Device)  g_Device.Reset();
		if (g_Factory) g_Factory.Reset();
	}


	GraphicsCommandKeyPtr DXDevice::RequestGraphicsCommandKey(int priority)
	{
		lock_guard<mutex> lock(g_CmdKeyMutex);
		auto key = make_shared<GraphicsCommandKey>();
		key->cmdResource = g_GraphicsFrameResource->GetExecuteCommandResource(priority);
		g_CommandKeyPool.push_back(key);
		return key;
	}
	ComputeCommandKeyPtr DXDevice::RequestComputeCommandKey(int priority)
	{
		lock_guard<mutex> lock(g_CmdKeyMutex);
		auto key = make_shared<ComputeCommandKey>();
		key->cmdResource = g_ComputeFrameResource->GetExecuteCommandResource(priority);
		g_CommandKeyPool.push_back(key);
		return key;
	}
	CopyCommandKeyPtr DXDevice::RequestCopyCommandKey(int priority)
	{
		lock_guard<mutex> lock(g_CmdKeyMutex);
		auto key = make_shared<CopyCommandKey>();
		key->cmdResource = g_CopyFrameResource->GetExecuteCommandResource(priority);
		g_CommandKeyPool.push_back(key);
		return key;
	}
	void       DXDevice::RegisterScreen(HWND hWnd, int width, int height,
		const DirectX::XMFLOAT4& clearColor)
	{
		auto screen = make_unique<Screen>(hWnd, width, height, clearColor);

		lock_guard<shared_mutex> lock(g_ScreenMutex);
		if (g_ScreenPool.find(hWnd) != g_ScreenPool.end()) return;
		g_ScreenPool[hWnd] = move(screen);
	}
	void DXDevice::RequestSubmitTextureToScreen(HWND hWnd, Texture& texture)
	{
		shared_lock<shared_mutex> lock(g_ScreenMutex);
		if (g_ScreenPool.find(hWnd) == g_ScreenPool.end()) return;
		g_ScreenPool[hWnd]->SubmitTexture(texture);
	}
	void DXDevice::RequestResizeScreen(HWND hWnd, int width, int height)
	{
		shared_lock<shared_mutex> lock(g_ScreenMutex);
		if (g_ScreenPool.find(hWnd) == g_ScreenPool.end()) return;
		ScreenInfo info;
		info.hWnd   = hWnd;
		info.width  = width;
		info.height = height;
		g_ResizingScreenQueue[GetBufferIndex()].push(info);
	}
	ScreenInfo DXDevice::GetScreenInfo(HWND hWnd)
	{
		Screen* screen = nullptr;
		shared_lock<shared_mutex> lock(g_ScreenMutex);
		if (g_ScreenPool.find(hWnd) == g_ScreenPool.end()) return ScreenInfo();
		return g_ScreenPool[hWnd].get()->GetScreenInfo();
	}

	template<typename _ShaderType_>
	_ShaderType_ CreateShaderFromFile(const string& path, const vector<pair<string, string>>& definedMacros)
	{
		std::ifstream fin;
		fin.open(path);

		if (!fin.is_open()) return _ShaderType_();

		fin.seekg(0, ios::end);
		size_t fileSize = fin.tellg();
		fin.seekg(0, ios::beg);

		std::string source_code;
		source_code.resize(fileSize);

		fin.read(&source_code[0], fileSize);
		fin.close();


		_ShaderType_ shader;
		shader.SetName(path);
		shader.SetSourceCode(source_code);

		for (auto& _macro : definedMacros)
		{
			shader.AddMacro(_macro.first, _macro.second);
		}
		shader.Compile(DX12_SHADER_ENTRY, DX12_SHADER_VERSION);
		return move(shader);	
	}
	VertexShader DXDevice::CreateVertexShaderFromFile(const std::string& path, const std::vector<std::pair<std::string, std::string>>& definedMacros)
	{
		return CreateShaderFromFile<VertexShader>(path, definedMacros);
	}
	HullShader DXDevice::CreateHullShaderFromFile(const std::string& path, const std::vector<std::pair<std::string, std::string>>& definedMacros)
	{
		return CreateShaderFromFile<HullShader>(path, definedMacros);
	}
	DomainShader DXDevice::CreateDomainShaderFromFile(const std::string& path, const std::vector<std::pair<std::string, std::string>>& definedMacros)
	{
		return CreateShaderFromFile<DomainShader>(path, definedMacros);
	}
	PixelShader DXDevice::CreatePixelShaderFromFile(const std::string& path, const std::vector<std::pair<std::string, std::string>>& definedMacros)
	{
		return CreateShaderFromFile<PixelShader>(path, definedMacros);
	}
	ComputeShader DXDevice::CreateComputeShaderFromFile(const std::string& path, const std::vector<std::pair<std::string, std::string>>& definedMacros)
	{
		return CreateShaderFromFile<ComputeShader>(path, definedMacros);
	}

	ID3D12Device* DXDevice::GetD3DDevice()
	{
		return g_Device.Get();
	}
	IDXGIFactory4* DXDevice::GetDXGIFactory()
	{
		return g_Factory.Get();
	}
	Texture DXDevice::LoadRegisteredTexture(const std::string& name)
	{

		auto iter = g_TextureCahce.find(name);
		if (iter == g_TextureCahce.end()) return Texture();
		else return iter->second;
	}
	uint64_t DXDevice::GetBufferIndex()
	{
		return g_BufferIndex;
	}
	// c



}

// DXCommand
namespace DX12
{
	void DXCommand::SetViewport(GraphicsCommandKeyPtr cmdKey, const Viewport& viewport)
	{
		//cmdKey->
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;


		cmdList->RSSetViewports(1, &viewport.Get());
	}
	void DXCommand::SetViewports(GraphicsCommandKeyPtr cmdKey, const std::vector<Viewport>& viewports)
	{
		vector<D3D12_VIEWPORT> views;
		for (auto& viewport : viewports)
		{
			views.push_back(viewport.Get());
		}
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;
		cmdList->RSSetViewports(views.size(), views.data());


	}
	void DXCommand::SetScissorRect(GraphicsCommandKeyPtr cmdKey, const ScissorRect& rect)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;


		cmdList->RSSetScissorRects(1, &rect.Get());
	}
	void DXCommand::SetScissorRects(GraphicsCommandKeyPtr cmdKey, const std::vector<ScissorRect>& rects)
	{
		vector<D3D12_RECT> d3d_rects;
		for (auto& rect : rects)
		{
			d3d_rects.push_back(rect.Get());
		}
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;
		cmdList->RSSetScissorRects(d3d_rects.size(), d3d_rects.data());
	}
	void DXCommand::ClearRenderTarget(GraphicsCommandKeyPtr cmdKey, RenderTarget& renderTarget)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;


		for (int i = 0; i < MAX_NUM_RENDERTARGET; ++i)
		{
			if (!renderTarget[i].IsVaild()) continue;

			TransitionBarrier(cmdKey, renderTarget[i], D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, false);
		}
		if (renderTarget.GetDepthStencilTexture().IsVaild())
		{
			TransitionBarrier(cmdKey, renderTarget.GetDepthStencilTexture(),
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, false);
		}


		FlushResourceBarrier(cmdKey);


		for (int i = 0; i < MAX_NUM_RENDERTARGET; ++i)
		{
			if (!renderTarget[i].IsVaild()) continue;

			auto clearValue = renderTarget[i].GetClearValue();
			auto rtvDesc = renderTarget.GetRTVDesc(i);
			if (clearValue)
			{
				cmdList->ClearRenderTargetView(renderTarget[i].GetRTV(rtvDesc), clearValue->Color, 0, nullptr);
			}
			else
			{
				float clearColor[4];
				clearColor[0] = 0.0f; clearColor[1] = 0.0f; clearColor[2] = 0.0f; clearColor[3] = 0.0f;
				cmdList->ClearRenderTargetView(renderTarget[i].GetRTV(rtvDesc), clearColor, 0, nullptr);
			}
			cmdResource->BackupResource(renderTarget[i]);
		}

		if (renderTarget.GetDepthStencilTexture().IsVaild())
		{
			float clearDepth   = 1.0f;
			uint8_t clearStencil = 0;
			auto clearValue = renderTarget.GetDepthStencilTexture().GetClearValue();
			auto dsvDesc = renderTarget.GetDSVDesc();
			if (clearValue)
			{
				clearDepth = clearValue->DepthStencil.Depth;
				clearStencil = clearValue->DepthStencil.Stencil;
			}
			cmdList->ClearDepthStencilView(renderTarget.GetDepthStencilTexture().GetDSV(dsvDesc),
				D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clearDepth, clearStencil, 0, nullptr);
			cmdResource->BackupResource(renderTarget.GetDepthStencilTexture());
		}
	}
	void DXCommand::SetRenderTarget(GraphicsCommandKeyPtr cmdKey, RenderTarget& renderTarget)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

		for (int i = 0; i < MAX_NUM_RENDERTARGET; ++i)
		{
			if (!renderTarget[i].IsVaild()) continue;

			TransitionBarrier(cmdKey, renderTarget[i], D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, false);
			auto rtvDesc = renderTarget.GetRTVDesc(i);

			rtvHandles.push_back(renderTarget[i].GetRTV(rtvDesc));
			cmdResource->BackupResource(renderTarget[i]);
		}
		if (renderTarget.GetDepthStencilTexture().IsVaild())
		{
			TransitionBarrier(cmdKey, renderTarget.GetDepthStencilTexture(),
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, false);

			auto dsvDesc = renderTarget.GetDSVDesc();
			dsvHandle = renderTarget.GetDepthStencilTexture().GetDSV(dsvDesc);
			cmdResource->BackupResource(renderTarget.GetDepthStencilTexture());
		}
		FlushResourceBarrier(cmdKey);

		if (!rtvHandles.empty())
		{
			if (dsvHandle.ptr != 0)
			{
				cmdList->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), false, &dsvHandle);
			}
			else
			{
				cmdList->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), false, nullptr);
			}
		}

	}
	void DXCommand::BindRootSignature(GraphicsCommandKeyPtr cmdKey, RootSignature& rootSig)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		if (cmdResource->bindedGraphicsRootSig.Get() == rootSig.GetD3DPtr()) return;

		cmdResource->bindedGraphicsRootSig = rootSig.GetD3DPtr();
		cmdList->SetGraphicsRootSignature(cmdResource->bindedGraphicsRootSig.Get());

		cmdResource->dynamicDescriptorAllocator->CommitRootSignature(rootSig);

	}
	void DXCommand::BindPipelineState(GraphicsCommandKeyPtr cmdKey, GraphicsPipelineState& pso)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		if (cmdResource->bindedGraphicsPSO.Get() == pso.GetD3DPtr()) return;


		cmdResource->bindedGraphicsPSO = pso.GetD3DPtr();
		cmdList->SetPipelineState(cmdResource->bindedGraphicsPSO.Get());
	}
	void BindTexture(CommandKeyPtr cmdKey, uint32_t rootParam, Texture& texture, void* desc)
	{
		if (!texture.IsVaild()) return;
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		// srv, uav, 
		int initType = cmdResource->dynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		D3D12_DESCRIPTOR_RANGE_TYPE tableType;
		// 0 CBV
		// 1
		switch (initType)
		{
		case RootSignature::__DescriptorTable__:
			tableType = cmdResource->dynamicDescriptorAllocator->GetDescriptorTableType(rootParam);
			switch (tableType)
			{
			case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
				cmdResource->dynamicDescriptorAllocator->CommitDescriptorTable(
					rootParam, { texture.GetSRV((D3D12_SHADER_RESOURCE_VIEW_DESC*)desc) });
				break;
			case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
				cmdResource->dynamicDescriptorAllocator->CommitDescriptorTable(
					rootParam, { texture.GetUAV((D3D12_UNORDERED_ACCESS_VIEW_DESC*)desc) });
				break;
			default:
				assert("trying bind CBV or Sampler in BindTexture");
				break;
			}
			break;
		default:
			assert("trying Create (CBV / Constant / SRV / UAV) in BindTexture");
			break;
			
		}

		cmdResource->BackupResource(texture);
	}
	void BindTextures(CommandKeyPtr cmdKey, uint32_t rootParam, 
		Texture* textures, void** desc, uint32_t textureCount)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		// srv, uav, 
		int initType = cmdResource->dynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		D3D12_DESCRIPTOR_RANGE_TYPE tableType;
		// 0 CBV
		// 1
		vector<D3D12_CPU_DESCRIPTOR_HANDLE> cpuHandles(textureCount);


		switch (initType)
		{
		case RootSignature::__DescriptorTable__:
			tableType = cmdResource->dynamicDescriptorAllocator->GetDescriptorTableType(rootParam);
			switch (tableType)
			{
			case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
				for (uint32_t i = 0; i < textureCount; ++i)
				{
					if (!textures[i].IsVaild()) continue;
					cpuHandles[i] = textures[i].GetSRV((D3D12_SHADER_RESOURCE_VIEW_DESC*)desc[i]);
					cmdResource->BackupResource(textures[i]);
				}
				cmdResource->dynamicDescriptorAllocator->CommitDescriptorTable(rootParam, cpuHandles);
				break;
			case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
				for (uint32_t i = 0; i < textureCount; ++i)
				{
					if (!textures[i].IsVaild()) continue;
					cpuHandles[i] = textures[i].GetUAV((D3D12_UNORDERED_ACCESS_VIEW_DESC*)desc[i]);
					cmdResource->BackupResource(textures[i]);
				}
				cmdResource->dynamicDescriptorAllocator->CommitDescriptorTable(rootParam, cpuHandles);
				break;
			default:
				assert("trying bind CBV or Sampler in BindTexture");
				break;
			}
			break;
		default:
			assert("trying Create (CBV / Constant / SRV / UAV) in BindTexture");
			break;

		}


	}

	void DXCommand::BindTexture(GraphicsCommandKeyPtr cmdKey, uint32_t rootParam, Texture& texture, void* desc)
	{
		DX12::BindTexture(cmdKey, rootParam, texture, desc);
	}

	void DXCommand::BindTextures(GraphicsCommandKeyPtr cmdKey, uint32_t rootParam, Texture* textures, void** desc, uint32_t textureCount)
	{
		DX12::BindTextures(cmdKey, rootParam, textures, desc, textureCount);
	
	}

	void DXCommand::BindConstantBuffer(GraphicsCommandKeyPtr cmdKey, uint32_t rootParam, ConstantBuffer& cBuffer)
	{
		if (!cBuffer.IsVaild()) return;
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;


		int initType = cmdResource->dynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		D3D12_DESCRIPTOR_RANGE_TYPE tableType;
		switch (initType)
		{
		case RootSignature::__DescriptorTable__:
			tableType = cmdResource->dynamicDescriptorAllocator->GetDescriptorTableType(rootParam);
			switch (tableType)
			{
			case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
				cmdResource->dynamicDescriptorAllocator->CommitDescriptorTable(rootParam, { cBuffer.GetCBV() });
				break;
			default:
				assert("trying bind SRV or UAV or Sampler in BindConstantBuffer");
				break;
			}
			
			break;
		case RootSignature::__ConstantBufferView__:
			cmdList->SetGraphicsRootConstantBufferView(rootParam, cBuffer.GetGPUVirtualAddress());
			break;
		case RootSignature::__ShaderResourceView__:
		case RootSignature::__UnorderedAccessView__:
		case RootSignature::__Constant__:
			assert("BindConstantBuffer not support ShaderResourceView / UnorderedAccessView / Constant");
			break;
		}
		cmdResource->BackupResource(cBuffer);
	}

	void DXCommand::BindDynamicConstantBuffer(GraphicsCommandKeyPtr cmdKey, uint32_t rootParam, void* data, size_t elementSize)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;


		int initType = cmdResource->dynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		auto alloc = cmdResource->uploadAllocator->Allocate(elementSize, 256);
		memcpy(alloc.CPU, data, elementSize);




		D3D12_DESCRIPTOR_RANGE_TYPE tableType;
		switch (initType)
		{
		case RootSignature::__DescriptorTable__:
			tableType = cmdResource->dynamicDescriptorAllocator->GetDescriptorTableType(rootParam);
			switch (tableType)
			{
			case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
				cbvDesc.BufferLocation = alloc.GPU;
				cbvDesc.SizeInBytes = (elementSize + 255) & ~255;
				
				auto cbvHandle = CSUAllocate();
				g_Device->CreateConstantBufferView(&cbvDesc, cbvHandle.CPU());
				cmdResource->dynamicDescriptorAllocator->CommitDescriptorTable(rootParam, { cbvHandle.CPU()});
			}
				break;
			default:
				assert("trying bind SRV or UAV or Sampler in BindConstantBuffer");
				break;
			}

			break;
		case RootSignature::__ConstantBufferView__:
			cmdList->SetGraphicsRootConstantBufferView(rootParam, alloc.GPU);
			break;
		case RootSignature::__ShaderResourceView__:
		case RootSignature::__UnorderedAccessView__:
		case RootSignature::__Constant__:
			assert("BindDynamicConstantBuffer not support ShaderResourceView / UnorderedAccessView / Constant");
			break;
		}
	}

	void DXCommand::BindStructuredBuffer(GraphicsCommandKeyPtr cmdKey, uint32_t rootParam, StructuredBuffer& buffer)
	{
		if (!buffer.IsVaild()) return;
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;


		int initType = cmdResource->dynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		D3D12_DESCRIPTOR_RANGE_TYPE tableType;
		switch (initType)
		{
		case RootSignature::__DescriptorTable__:
			tableType = cmdResource->dynamicDescriptorAllocator->GetDescriptorTableType(rootParam);
			switch (tableType)
			{
			case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
				cmdResource->dynamicDescriptorAllocator->CommitDescriptorTable(rootParam, { buffer.GetSRV() });
				break;
			case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
				cmdResource->dynamicDescriptorAllocator->CommitDescriptorTable(rootParam, { buffer.GetUAV() });
				break;
			default:
				assert("trying bind CBV or Sampler in BindStructuredBuffer");
				break;
			}
			break;
		case RootSignature::__ShaderResourceView__:
			cmdList->SetGraphicsRootShaderResourceView(rootParam, buffer.GetGPUVirtualAddress());
			break;
		case RootSignature::__UnorderedAccessView__:
			TransitionBarrier(cmdKey, buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			cmdList->SetGraphicsRootUnorderedAccessView(rootParam, buffer.GetGPUVirtualAddress());
			break;
		case RootSignature::__ConstantBufferView__:
		case RootSignature::__Constant__:
			assert("BindStructuredBuffer not support ConstantBufferView / Constant");
			break;
		}
		cmdResource->BackupResource(buffer);
	}

	void DXCommand::BindDynamicStructuredBuffer(GraphicsCommandKeyPtr cmdKey, uint32_t rootParam, void* data, size_t elementSize, size_t elementCount)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;


		size_t btSize = elementSize * elementCount;


		auto alloc = cmdResource->uploadAllocator->Allocate(btSize, elementSize);
		memcpy(alloc.CPU, data, btSize);

		int initType = cmdResource->dynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		D3D12_DESCRIPTOR_RANGE_TYPE tableType;
		switch (initType)
		{
		case RootSignature::__ShaderResourceView__:
			cmdList->SetGraphicsRootShaderResourceView(rootParam, alloc.GPU);
			break;
		case RootSignature::__UnorderedAccessView__:
			cmdList->SetGraphicsRootUnorderedAccessView(rootParam, alloc.GPU);
			break;
		default:
			assert("BindDynamicStructuredBuffer not support ConstantBufferView / Constant / DescriptorTable");
			break;
		}



	}

	void DXCommand::BindConstants(GraphicsCommandKeyPtr cmdKey, uint32_t rootParam, uint32_t btSize, void* data, uint32_t offset)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		int initType = cmdResource->dynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		D3D12_DESCRIPTOR_RANGE_TYPE tableType;
		switch (initType)
		{
		case RootSignature::__Constant__:
			cmdList->SetGraphicsRoot32BitConstants(rootParam, btSize / 4, data, offset / 4);
			break;
		default:
			assert("BindConstants not support CBV / SRV / UAV /DescriptorTable");
			break;
		}
	}


	void DXCommand::BindVertexBuffer(GraphicsCommandKeyPtr cmdKey, VertexBuffer& vBuffer)
	{
		if (!vBuffer.IsVaild()) return;
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;
		if (!vBuffer.IsVaild()) return;

		TransitionBarrier(cmdKey, vBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		cmdList->IASetVertexBuffers(0, 1, &vBuffer.Get());
		cmdResource->BackupResource(vBuffer);
	}
	void DXCommand::BindDynamicVertexBuffer(GraphicsCommandKeyPtr cmdKey, void* data, size_t elementSize, size_t elementCount)
	{

		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		size_t btSize = elementSize * elementCount;
		auto alloc = cmdResource->uploadAllocator->Allocate(btSize, elementSize);

		memcpy(alloc.CPU, data, btSize);

		D3D12_VERTEX_BUFFER_VIEW bufferView;
		bufferView.BufferLocation = alloc.GPU;
		bufferView.SizeInBytes = btSize;
		bufferView.StrideInBytes = elementSize;

		cmdList->IASetVertexBuffers(0, 1, &bufferView);


	}





	void DXCommand::BindIndexBuffer(GraphicsCommandKeyPtr cmdKey, IndexBuffer& iBuffer)
	{
		if (!iBuffer.IsVaild()) return;
			
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;
		if (!iBuffer.IsVaild()) return;

		TransitionBarrier(cmdKey, iBuffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);

		cmdList->IASetIndexBuffer(&iBuffer.Get());
		cmdResource->BackupResource(iBuffer);
	}
	void DXCommand::DynamicBindIndexBuffer(GraphicsCommandKeyPtr cmdKey, const std::vector<uint32_t>& indices)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		size_t btSize = indices.size() & sizeof(uint32_t);
		auto alloc = cmdResource->uploadAllocator->Allocate(btSize, sizeof(uint32_t));

		memcpy(alloc.CPU, indices.data(), btSize);

		D3D12_INDEX_BUFFER_VIEW bufferView;
		bufferView.BufferLocation = alloc.GPU;
		bufferView.Format = DXGI_FORMAT_R32_UINT;
		bufferView.SizeInBytes = btSize;
		cmdList->IASetIndexBuffer(&bufferView);

	}

	void DXCommand::SetPrimitiveTopology(GraphicsCommandKeyPtr cmdKey, D3D_PRIMITIVE_TOPOLOGY topology)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		cmdList->IASetPrimitiveTopology(topology);

	}

	void DXCommand::Draw(GraphicsCommandKeyPtr cmdKey,
		uint32_t vertexCount, uint32_t instanceCount, 
		uint32_t startVertexLocation, uint32_t startInstanceLocation)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;
		cmdResource->dynamicDescriptorAllocator->PushDescriptorTable(cmdResource, true);
		cmdList->DrawInstanced(vertexCount, instanceCount, startVertexLocation, startInstanceLocation);


	
	}

	void DXCommand::DrawIndexed(GraphicsCommandKeyPtr cmdKey,
		uint32_t indexCount, uint32_t instanceCount,
		uint32_t startIndexLocation, uint32_t startVertexLocation, uint32_t startInstanceLocation)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;
		cmdResource->dynamicDescriptorAllocator->PushDescriptorTable(cmdResource, true);
		
		cmdList->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, startVertexLocation, startInstanceLocation);

	}

	void DXCommand::BindRootSignature(ComputeCommandKeyPtr cmdKey, RootSignature& rootSig)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		if (cmdResource->bindedComputeRootSig.Get() == rootSig.GetD3DPtr()) return;

		cmdResource->bindedComputeRootSig = rootSig.GetD3DPtr();
		cmdList->SetComputeRootSignature(cmdResource->bindedComputeRootSig.Get());

		cmdResource->dynamicDescriptorAllocator->CommitRootSignature(rootSig);
	}

	void DXCommand::BindPipelineState(ComputeCommandKeyPtr cmdKey, ComputePipelineState& pso)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		if (cmdResource->bindedComputePSO.Get() == pso.GetD3DPtr()) return;


		cmdResource->bindedComputePSO = pso.GetD3DPtr();
		cmdList->SetPipelineState(cmdResource->bindedComputePSO.Get());
	}



	void DXCommand::BindTexture(ComputeCommandKeyPtr cmdKey, uint32_t rootParam, Texture& texture, void* desc)
	{
		DX12::BindTexture(cmdKey, rootParam, texture, desc);
	}

	void DXCommand::BindTextures(ComputeCommandKeyPtr cmdKey, uint32_t rootParam, Texture* textures, void** desc, uint32_t textureCount)
	{
		DX12::BindTextures(cmdKey, rootParam, textures, desc, textureCount);
	}

	void DXCommand::BindConstantBuffer(ComputeCommandKeyPtr cmdKey, uint32_t rootParam, ConstantBuffer& buffer)
	{
		if (!buffer.IsVaild()) return;
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;


		int initType = cmdResource->dynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		D3D12_DESCRIPTOR_RANGE_TYPE tableType;
		switch (initType)
		{
		case RootSignature::__DescriptorTable__:
			tableType = cmdResource->dynamicDescriptorAllocator->GetDescriptorTableType(rootParam);
			switch (tableType)
			{
			case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
				cmdResource->dynamicDescriptorAllocator->CommitDescriptorTable(rootParam, { buffer.GetCBV() });
				break;
			default:
				assert("trying bind SRV or UAV or Sampler in BindConstantBuffer");
				break;
			}

			break;
		case RootSignature::__ConstantBufferView__:
			cmdList->SetComputeRootConstantBufferView(rootParam, buffer.GetGPUVirtualAddress());
			break;
		case RootSignature::__ShaderResourceView__:
		case RootSignature::__UnorderedAccessView__:
		case RootSignature::__Constant__:
			assert("BindConstantBuffer not support ShaderResourceView / UnorderedAccessView / Constant");
			break;
		}
		cmdResource->BackupResource(buffer);
	}

	void DXCommand::BindDynamicConstantBuffer(ComputeCommandKeyPtr cmdKey, uint32_t rootParam, void* data, size_t elementSize)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;


		int initType = cmdResource->dynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		auto alloc = cmdResource->uploadAllocator->Allocate(elementSize, 256);
		memcpy(alloc.CPU, data, elementSize);




		D3D12_DESCRIPTOR_RANGE_TYPE tableType;
		switch (initType)
		{
		case RootSignature::__DescriptorTable__:
			tableType = cmdResource->dynamicDescriptorAllocator->GetDescriptorTableType(rootParam);
			switch (tableType)
			{
			case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
				cbvDesc.BufferLocation = alloc.GPU;
				cbvDesc.SizeInBytes = (elementSize + 255) & ~255;

				auto cbvHandle = CSUAllocate();
				g_Device->CreateConstantBufferView(&cbvDesc, cbvHandle.CPU());
				cmdResource->dynamicDescriptorAllocator->CommitDescriptorTable(rootParam, { cbvHandle.CPU() });
			}
			break;
			default:
				assert("trying bind SRV or UAV or Sampler in BindConstantBuffer");
				break;
			}

			break;
		case RootSignature::__ConstantBufferView__:
			cmdList->SetComputeRootConstantBufferView(rootParam, alloc.GPU);
			break;
		case RootSignature::__ShaderResourceView__:
		case RootSignature::__UnorderedAccessView__:
		case RootSignature::__Constant__:
			assert("BindDynamicConstantBuffer not support ShaderResourceView / UnorderedAccessView / Constant");
			break;
		}
	}

	void DXCommand::BindStructuredBuffer(ComputeCommandKeyPtr cmdKey, uint32_t rootParam, StructuredBuffer& buffer)
	{
		if (!buffer.IsVaild()) return;
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;


		int initType = cmdResource->dynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		D3D12_DESCRIPTOR_RANGE_TYPE tableType;
		switch (initType)
		{
		case RootSignature::__DescriptorTable__:
			tableType = cmdResource->dynamicDescriptorAllocator->GetDescriptorTableType(rootParam);
			switch (tableType)
			{
			case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
				cmdResource->dynamicDescriptorAllocator->CommitDescriptorTable(rootParam, { buffer.GetSRV() });
				break;
			case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
				cmdResource->dynamicDescriptorAllocator->CommitDescriptorTable(rootParam, { buffer.GetUAV() });
				break;
			default:
				assert("trying bind CBV or Sampler in BindStructuredBuffer");
				break;
			}
			break;
		case RootSignature::__ShaderResourceView__:
			cmdList->SetComputeRootShaderResourceView(rootParam, buffer.GetGPUVirtualAddress());
			break;
		case RootSignature::__UnorderedAccessView__:
			TransitionBarrier(cmdKey, buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			cmdList->SetComputeRootUnorderedAccessView(rootParam, buffer.GetGPUVirtualAddress());
			break;
		case RootSignature::__ConstantBufferView__:
		case RootSignature::__Constant__:
			assert("BindStructuredBuffer not support ConstantBufferView / Constant");
			break;
		}
		cmdResource->BackupResource(buffer);
	}

	void DXCommand::BindDynamicStructuredBuffer(ComputeCommandKeyPtr cmdKey, uint32_t rootParam, void* data, size_t elementSize, size_t elementCount)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;


		size_t btSize = elementSize * elementCount;


		auto alloc = cmdResource->uploadAllocator->Allocate(btSize, elementSize);
		memcpy(alloc.CPU, data, btSize);

		int initType = cmdResource->dynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		D3D12_DESCRIPTOR_RANGE_TYPE tableType;
		switch (initType)
		{
		case RootSignature::__ShaderResourceView__:
			cmdList->SetComputeRootShaderResourceView(rootParam, alloc.GPU);
			break;
		case RootSignature::__UnorderedAccessView__:
			cmdList->SetComputeRootUnorderedAccessView(rootParam, alloc.GPU);
			break;
		default:
			assert("BindDynamicStructuredBuffer not support ConstantBufferView / Constant / DescriptorTable");
			break;
		}

	}
	void DXCommand::BindConstants(ComputeCommandKeyPtr cmdKey, uint32_t rootParam, uint32_t btSize, void* data, uint32_t offset)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		int initType = cmdResource->dynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		D3D12_DESCRIPTOR_RANGE_TYPE tableType;
		switch (initType)
		{
		case RootSignature::__Constant__:
			cmdList->SetComputeRoot32BitConstants(rootParam, btSize / 4, data, offset / 4);
			break;
		default:
			assert("BindConstants not support CBV / SRV / UAV /DescriptorTable");
			break;
		}
	}
	void DXCommand::Dispatch(ComputeCommandKeyPtr cmdKey, uint32_t xgroupCount, uint32_t ygroupCount, uint32_t zgroupCount)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		cmdResource->dynamicDescriptorAllocator->PushDescriptorTable(cmdResource, false);
		cmdList->Dispatch(xgroupCount, ygroupCount, zgroupCount);

	}
	bool DXCommand::LoadTextureFromFile(CommandKeyPtr cmdKey, const std::string& path, bool is_cube)
	{
		HRESULT hr = S_OK;
		{
			shared_lock<shared_mutex> readLock(g_LoadingTextureMutex);
			if (g_LoadingTextureCache.find(path) != g_LoadingTextureCache.end() ||
				g_TextureCahce.find(path) != g_TextureCahce.end())
			{
				readLock.unlock();
				return true;
			}
		}
		

		fs::path p(path);


		
		TexMetadata metadata;
		ScratchImage scratchImage;

		if (p.extension() == ".dds")
		{
			hr = LoadFromDDSFile(p.wstring().c_str(),
				DDS_FLAGS_NONE,
				&metadata,
				scratchImage);

		}
		else if (p.extension() == ".hdf")
		{
			hr = LoadFromHDRFile(p.wstring().c_str(),
				&metadata,
				scratchImage);
		}
		else if (p.extension() == ".tga")
		{
			hr = LoadFromTGAFile(p.wstring().c_str(),
				&metadata,
				scratchImage);
		}
		else
		{
			CoInitialize(nullptr);
			hr = LoadFromWICFile(p.wstring().c_str(),
				DDS_FLAGS_NONE,
				&metadata,
				scratchImage);
		}
		if (FAILED(hr))
		{
			return false;
		}
		D3D12_RESOURCE_DESC desc = {};

		switch (metadata.dimension)
		{
		case TEX_DIMENSION_TEXTURE1D:
			desc = CD3DX12_RESOURCE_DESC::Tex1D(
				metadata.format,
				static_cast<UINT64>(metadata.width),
				static_cast<UINT16>(metadata.arraySize));
			break;
		case TEX_DIMENSION_TEXTURE2D:
			desc = CD3DX12_RESOURCE_DESC::Tex2D(
				metadata.format,
				static_cast<UINT64>(metadata.width),
				static_cast<UINT>(metadata.height),
				static_cast<UINT16>(metadata.arraySize));
			break;
		case TEX_DIMENSION_TEXTURE3D:
			desc = CD3DX12_RESOURCE_DESC::Tex3D(
				metadata.format,
				static_cast<UINT64>(metadata.width),
				static_cast<UINT>(metadata.height),
				static_cast<UINT16>(metadata.depth));
			break;
		default:
			return false;
		}

		desc.MipLevels = scratchImage.GetImageCount() / metadata.arraySize;
		Texture load_texture;
		load_texture.Create(path, desc);




		std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratchImage.GetImageCount());

		const Image* pImages = scratchImage.GetImages();


		for (int i = 0; i < scratchImage.GetImageCount(); ++i)
		{
			auto& subresource = subresources[i];
			subresource.RowPitch = pImages[i].rowPitch;
			subresource.SlicePitch = pImages[i].slicePitch;
			subresource.pData = pImages[i].pixels;
		}
		auto cmdList = cmdKey->cmdResource->d3d_commandList;
		

		TransitionBarrier(cmdKey, load_texture, D3D12_RESOURCE_STATE_COPY_DEST);
		UINT64 requiredSize = GetRequiredIntermediateSize(load_texture.GetD3DPtr(), 0, subresources.size());

		UploadBuffer uploadBuffer;
		uploadBuffer.Create(path + "_UploadBuffer", requiredSize);
		UpdateSubresources(cmdList.Get(), load_texture.GetD3DPtr(), uploadBuffer.GetD3DPtr(),
			0, 0, subresources.size(), subresources.data());


		cmdKey->cmdResource->BackupResource(uploadBuffer);
		cmdKey->cmdResource->BackupResource(load_texture);


		{
			unique_lock<shared_mutex> writelock(g_LoadingTextureMutex);
			g_LoadingTextureCache[path] = pair<uint32_t, Texture>(DXDevice::GetBufferIndex(), load_texture);
		}



		return true;
	}

	void DXCommand::CopyResource(CommandKeyPtr cmdKey, Resource& dest, Resource& src)
	{
		if (!dest.IsVaild() || !src.IsVaild()) return;
		TransitionBarrier(cmdKey, dest, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, false);
		TransitionBarrier(cmdKey, src, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, false);
		FlushResourceBarrier(cmdKey);


		auto cmdResource = cmdKey->cmdResource;
		auto cmdList     = cmdResource->d3d_commandList;

		cmdList->CopyResource(dest.GetD3DPtr(), src.GetD3DPtr());


		cmdResource->BackupResource(dest);
		cmdResource->BackupResource(src);
	}

	void DXCommand::CopyBuffer(CommandKeyPtr cmdKey, Buffer& buffer, void* data, size_t elementSize, size_t elementCount)
	{
		if (!buffer.IsVaild()) return;
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		size_t btSize = elementSize * elementCount;



		UploadBuffer uploadBuffer;
		uploadBuffer.Create(buffer.GetName() + "_UploadBuffer", (uint32_t)btSize);

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = data;
		subResourceData.RowPitch = btSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;


		TransitionBarrier(cmdKey, buffer, D3D12_RESOURCE_STATE_COPY_DEST);
		UpdateSubresources(cmdList.Get(), buffer.GetD3DPtr(), uploadBuffer.GetD3DPtr(), 0, 0, 1, &subResourceData);
		cmdResource->BackupResource(buffer);
		cmdResource->BackupResource(uploadBuffer);
	}

	void DXCommand::CopyConstantBuffer(CommandKeyPtr cmdKey, ConstantBuffer& buffer, void* data, size_t elementSize)
	{
		if (!buffer.IsVaild()) return;
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;
		size_t btSize = (elementSize + 255) & ~255;
		CopyBuffer(cmdKey, buffer, data, btSize, 1);
		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;
		Desc.BufferLocation = buffer.GetGPUVirtualAddress();
		Desc.SizeInBytes = btSize;
		if (!buffer.m_CBV.IsVaild())
		{
			buffer.m_CBV = CSUAllocate();
		}
		g_Device->CreateConstantBufferView(&Desc, buffer.m_CBV.CPU());
	}

	void DXCommand::CopyStructuredBuffer(CommandKeyPtr cmdKey, StructuredBuffer& buffer, void* data, size_t elementSize, size_t elementCount)
	{
		if (!buffer.IsVaild()) return;
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		size_t btSize = (elementSize * elementCount);
		CopyBuffer(cmdKey, buffer, data, elementSize, elementCount);
		buffer.m_ElementCount = elementCount;
		buffer.m_ElementSize  = elementSize;
		if (!buffer.m_SRV.IsVaild())
		{
			buffer.m_SRV = CSUAllocate();

		}
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = (uint32_t)elementCount;
		srvDesc.Buffer.StructureByteStride = (uint32_t)elementSize;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		g_Device->CreateShaderResourceView(buffer.GetD3DPtr(), &srvDesc, buffer.m_SRV.CPU());

		if (buffer.GetDesc().Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
		{
			if (!buffer.m_UAV.IsVaild()) buffer.m_UAV = CSUAllocate();
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.Buffer.CounterOffsetInBytes = 0;
			uavDesc.Buffer.NumElements = (uint32_t)elementCount;
			uavDesc.Buffer.StructureByteStride = (uint32_t)elementSize;
			uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

			g_Device->CreateUnorderedAccessView(buffer.GetD3DPtr(), nullptr, &uavDesc, buffer.m_UAV.CPU());
		}
	}

	void DXCommand::CopyVertexBuffer(CommandKeyPtr cmdKey, VertexBuffer& vBuffer, void* data, size_t elementSize, size_t elementCount)
	{
		if (!vBuffer.IsVaild()) return;
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		size_t btSize = elementSize * elementCount;

		

		CopyBuffer(cmdKey, vBuffer, data, elementSize, elementCount);
		D3D12_VERTEX_BUFFER_VIEW d3d_vertex_view;

		d3d_vertex_view.BufferLocation = vBuffer.GetGPUVirtualAddress();
		d3d_vertex_view.SizeInBytes = btSize;
		d3d_vertex_view.StrideInBytes = (uint32_t)elementSize;
		vBuffer.m_View = d3d_vertex_view;
		vBuffer.m_ElementSize = elementSize;
		vBuffer.m_ElmenetCount = elementCount;
		cmdResource->BackupResource(vBuffer);
	}
	void  DXCommand::CopyIndexBuffer(CommandKeyPtr cmdKey, IndexBuffer& iBuffer, void* data, size_t elementCount)
	{
		if (!iBuffer.IsVaild()) return;
		auto cmdResource = cmdKey->cmdResource;
		auto cmdList = cmdResource->d3d_commandList;

		size_t btSize = sizeof(uint32_t) * elementCount;


		CopyBuffer(cmdKey, iBuffer, data, sizeof(uint32_t), elementCount);


		D3D12_INDEX_BUFFER_VIEW d3d_index_view;

		d3d_index_view.BufferLocation = iBuffer.GetGPUVirtualAddress();
		d3d_index_view.SizeInBytes = (uint32_t)btSize;
		d3d_index_view.Format = DXGI_FORMAT_R32_UINT;
		iBuffer.m_View = d3d_index_view;
		iBuffer.m_ElementSize = sizeof(uint32_t);
		iBuffer.m_ElementCount = elementCount;
		cmdResource->BackupResource(iBuffer);
	}

	void DXCommand::FlushResourceBarrier(CommandKeyPtr cmdKey)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto& resourceTracker = cmdResource->resourceStateTracker;
		resourceTracker->FlushResourceBarrier(cmdResource->d3d_commandList.Get());
	}
	void DXCommand::TransitionBarrier(
		CommandKeyPtr cmdKey, const Resource& rsc, D3D12_RESOURCE_STATES state,
		uint32_t subresource, bool flush)
	{
		if (!rsc.IsVaild()) return;
		auto cmdResource = cmdKey->cmdResource;
		auto& resourceTracker = cmdResource->resourceStateTracker;

		resourceTracker->TransitionBarrier(rsc, state, subresource);
		if (flush)
		{
			FlushResourceBarrier(cmdKey);
		}
		cmdResource->BackupResource(rsc);
	}

	void DXCommand::UAVBarrier(CommandKeyPtr cmdKey, const Resource& rsc, bool flush)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto& resourceTracker = cmdResource->resourceStateTracker;

		resourceTracker->UAVBarrier(rsc);
		if (flush)
		{
			FlushResourceBarrier(cmdKey);
		}
		cmdResource->BackupResource(rsc);
	}
	void DXCommand::AliasBarrier(CommandKeyPtr cmdKey, const Resource& before_rsc, const Resource& after_rsc, bool flush)
	{
		auto cmdResource = cmdKey->cmdResource;
		auto& resourceTracker = cmdResource->resourceStateTracker;

		resourceTracker->AliasBarrier(before_rsc, after_rsc);
		if (flush)
		{
			FlushResourceBarrier(cmdKey);
		}
		cmdResource->BackupResource(before_rsc);
		cmdResource->BackupResource(after_rsc);
	}

}


// DX12 ï¿½ï¿½Ä¡ Å¬ï¿½ï¿½ï¿½ï¿½
namespace DX12
{
	Fence::Fence() :
		m_FenceValue(0)
	{
		m_FenceEvent = ::CreateEvent(nullptr, false, false, nullptr);
		assert(m_FenceEvent && "Failed to create fence event.");


		HRESULT hr = DXDevice::GetD3DDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_D3D_Fence));
		assert(SUCCEEDED(hr) && "failed Create Fence Object");

	}
	Fence::~Fence()
	{
		::CloseHandle(m_FenceEvent);
	}
	void Fence::WaitForFenceValue(uint64_t value, milliseconds duration)
	{
		std::lock_guard<std::mutex> lock(m_EventMutex);

		if (m_D3D_Fence->GetCompletedValue() < value)
		{
			HRESULT hr = m_D3D_Fence->SetEventOnCompletion(value, m_FenceEvent);
			assert(SUCCEEDED(hr) && "failed SetEventOnCompletion in Fence Object");
			::WaitForSingleObject(m_FenceEvent, static_cast<DWORD>(duration.count()));
		}
	}



	DescriptorAllocation DescriptorAllocator::Allocate()
	{
		lock_guard<mutex> lock(m_AllocateMutex);
		for (auto& page : m_Pages)
		{
			if (page->HasSpace())
			{
				return move(page->Allocate());
			}
		}


		auto page = RequestPage();

		return move(page->Allocate());
	}
	void DescriptorAllocator::UpdatePage()
	{
		for (auto& page : m_Pages)
		{
			page->UpdatePage();
		}
	}
	DescriptorAllocator::CPUDescriptorPage* DescriptorAllocator::RequestPage()
	{
		size_t i = m_Pages.size();
		auto page = make_unique<CPUDescriptorPage>();
		auto result = page.get();
		page->D3DType = m_D3DType;
		page->HeapSize = m_SizePerHep;
		page->D3DDescriptorHeap = CreateD3DDescriptorHeap(g_Device, m_D3DType,
			D3D12_DESCRIPTOR_HEAP_FLAG_NONE, page->HeapSize);
		m_Pages.push_back(move(page));

		return result;
	}
	bool DescriptorAllocator::CPUDescriptorPage::HasSpace()
	{
		if (!AbandonedHandles.empty())
		{
			return true;
		}
		if (Offset >= HeapSize) return false;
		else return true;
	}
	DescriptorAllocation DescriptorAllocator::CPUDescriptorPage::Allocate()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpu_handle;
		if (!AbandonedHandles.empty())
		{
			cpu_handle = move(AbandonedHandles.front()); AbandonedHandles.pop();
		}
		else
		{
			cpu_handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3DDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			cpu_handle.Offset((uint32_t)Offset++, DXDevice::GetD3DDevice()->GetDescriptorHandleIncrementSize(D3DType));
		}
		DescriptorAllocation alloc;
		alloc._CPU = cpu_handle;
		alloc._OwnerPage = this;

		return move(alloc);
	}

	void DescriptorAllocator::CPUDescriptorPage::UpdatePage()
	{
		uint64_t device_buffer_index = DXDevice::GetBufferIndex();
		while (!PendingHandles[device_buffer_index].empty())
		{
			AbandonedHandles.push(PendingHandles[device_buffer_index].front());
			PendingHandles[device_buffer_index].pop();
		}
	}

	void DescriptorAllocator::CPUDescriptorPage::Free(D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		lock_guard<mutex> lock(FreeMutex);
		uint64_t device_buffer_index = DXDevice::GetBufferIndex();

		PendingHandles[device_buffer_index].push(handle);
	}

	DescriptorAllocation::~DescriptorAllocation()
	{
		if (_OwnerPage)
		{
			_OwnerPage->Free(_CPU);
		}

	}

	DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& rhs)
	{
		_CPU = rhs._CPU;
		_OwnerPage = rhs._OwnerPage;

		rhs._CPU.ptr = 0;
		rhs._OwnerPage = nullptr;
	}
	DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& rhs)
	{
		_CPU = rhs._CPU;
		_OwnerPage = rhs._OwnerPage;

		rhs._CPU.ptr = 0;
		rhs._OwnerPage = nullptr;
		return *this;
	}
	UploadAllocator::Page::Page(ID3D12Device* device, size_t sizeInBytes) :
		m_PageSize(sizeInBytes),
		m_Offset(0),
		m_CPUptr(nullptr),
		m_GPUptr(D3D12_GPU_VIRTUAL_ADDRESS(0))
	{
		HRESULT hr = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_PageSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(m_D3D12Resource.GetAddressOf()));

		m_GPUptr = m_D3D12Resource->GetGPUVirtualAddress();
		m_D3D12Resource->Map(0, nullptr, &m_CPUptr);
	}
	UploadAllocator::Page::~Page()
	{
		m_D3D12Resource->Unmap(0, nullptr);
		m_CPUptr = nullptr;
		m_GPUptr = D3D12_GPU_VIRTUAL_ADDRESS(0);
	}

	bool UploadAllocator::Page::HasSpace(size_t sizeInBytes, size_t alignment) const
	{
		size_t alignedSize = (sizeInBytes + (alignment - 1)) & ~(alignment - 1);
		size_t alignedOffset = (m_Offset + (alignment - 1)) & ~(alignment - 1);

		return alignedOffset + alignedSize <= m_PageSize;
	}

	UploadAllocator::Allocation UploadAllocator::Page::Allocate(size_t sizeInBytes, size_t alignment)
	{
		if (!HasSpace(sizeInBytes, alignment))
		{
			throw std::bad_alloc();
		}

		size_t alignedSize = (sizeInBytes + (alignment - 1)) & ~(alignment - 1);
		m_Offset = (m_Offset + (alignment - 1)) & ~(alignment - 1);

		Allocation allocation;
		allocation.CPU = static_cast<uint8_t*>(m_CPUptr) + m_Offset;
		allocation.GPU = m_GPUptr + m_Offset;

		m_Offset += alignedSize;

		return allocation;
	}
	void UploadAllocator::Page::Reset()
	{
		m_Offset = 0;
	}


	UploadAllocator::UploadAllocator(size_t pageSize) :
		m_PageSize(pageSize) { }
	UploadAllocator::~UploadAllocator() {}

	UploadAllocator::Allocation UploadAllocator::Allocate(size_t sizeInBytes, size_t alignment)
	{
		if (sizeInBytes > m_PageSize)
		{
			throw std::bad_alloc();
		}

		if (!m_CurrentPage || !m_CurrentPage->HasSpace(sizeInBytes, alignment))
		{
			m_CurrentPage = RequestPage();
		}

		return m_CurrentPage->Allocate(sizeInBytes, alignment);
	}
	void UploadAllocator::Reset()
	{
		m_CurrentPage = nullptr;
		m_AvailablePages = m_PagePool;

		for (auto& page : m_AvailablePages)
		{
			page->Reset();
		}
	}

	std::shared_ptr<UploadAllocator::Page> UploadAllocator::RequestPage()
	{
		std::shared_ptr<Page> page;

		if (!m_AvailablePages.empty())
		{
			page = m_AvailablePages.front();
			m_AvailablePages.pop_front();
		}
		else
		{
			page = make_shared<Page>(g_Device.Get(), m_PageSize);
			m_PagePool.push_back(page);
		}
		return page;
	}



	DynamicDescriptorAllocator::DynamicDescriptorAllocator(uint32_t numDescriptor)
	{
		m_IncreaseSize = g_Device->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);
	}

	void DynamicDescriptorAllocator::CommitRootSignature(RootSignature& rootSig)
	{
		
		Reset();
		for (int i = 0; i < (int)rootSig.m_RootSigInitType.size(); ++i)
		{
			m_RootParamInitTypeMap[i] = rootSig.m_RootSigInitType[i];
		}
	
		for (auto& info_pair : rootSig.m_DescriptorTableInfoByRootParam)
		{
			uint32_t rootParam = info_pair.first;
			uint32_t numDescriptor = info_pair.second.numDescirptor;
			auto     rangeType = info_pair.second.type;

			m_DescriptorTableType[rootParam]    = rangeType;
			m_CPUCache[rootParam].numDescriptor = numDescriptor;
			m_CommittedNumDescriptor += numDescriptor;
		}

		RequestDescriptorHeap();


		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_CurrentD3DHeap.DescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_CurrentD3DHeap.DescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		for (auto& cache_pair : m_CPUCache)
		{
			uint32_t rootParam = cache_pair.first;
			auto& cache = cache_pair.second;

			cache.startCPU = cpuHandle;
			cache.startGPU = gpuHandle;



			cpuHandle.Offset(cache.numDescriptor, m_IncreaseSize);
			gpuHandle.Offset(cache.numDescriptor, m_IncreaseSize);
		}
	}
	void DynamicDescriptorAllocator::CommitDescriptorTable(uint32_t rootParam, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)
	{
		auto& cpuHandles = m_CPUCache[rootParam].cpuHandles;
		cpuHandles.insert(cpuHandles.end(), handles.begin(), handles.end());
		m_CommittedCPUHandleCount += (uint32_t)handles.size();
	}
	void DynamicDescriptorAllocator::Reset()
	{
		m_CommittedNumDescriptor = 0;
		m_CommittedCPUHandleCount = 0;
		m_RootParamInitTypeMap.clear();
		m_DescriptorTableType.clear();
	
		m_CPUCache.clear();
	}


	void DynamicDescriptorAllocator::PushDescriptorTable(_DXCommandResource* cmdResource, bool is_graphics)
	{
		auto cmdList = cmdResource->d3d_commandList;
		if (m_CommittedCPUHandleCount > m_CurrentD3DHeap.numDescriptor)
		{
			assert("Committed CPUHandleCount is higher than Committed RootSignature DescriptorTable numDescriptor");
		}
		if (cmdResource->bindedDescriptorHeap != m_CurrentD3DHeap.DescriptorHeap)
		{
			cmdResource->bindedDescriptorHeap = m_CurrentD3DHeap.DescriptorHeap;
			cmdList->SetDescriptorHeaps(1, cmdResource->bindedDescriptorHeap.GetAddressOf());
		}
		if (!m_CPUCache.empty())
		{
			vector<D3D12_CPU_DESCRIPTOR_HANDLE> cpu_Handles;

			for (auto& cache_pair : m_CPUCache)
			{
				int  rootParam = cache_pair.first;
				auto handles = cache_pair.second.cpuHandles.data();
				uint32_t handleCount = (uint32_t)cache_pair.second.cpuHandles.size();


				auto startGPU = cache_pair.second.startGPU;
				auto startCPU = cache_pair.second.startCPU;

				vector<uint32_t> srcDescriptorRangeSize(handleCount, 1);

				g_Device->CopyDescriptors(
					1, &startCPU, &handleCount,
					handleCount, handles, srcDescriptorRangeSize.data(),
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (is_graphics)
				{
					cmdList->SetGraphicsRootDescriptorTable(rootParam, startGPU);
				}
				else
				{
					cmdList->SetComputeRootDescriptorTable(rootParam, startGPU);
				}
				cache_pair.second.cpuHandles.clear();
			}

		}

	
		m_CommittedCPUHandleCount = 0;


	}
	void DynamicDescriptorAllocator::RequestDescriptorHeap()
	{
		if (m_CurrentD3DHeap.numDescriptor >= m_CommittedNumDescriptor && m_CurrentD3DHeap.DescriptorHeap) return;


		m_CurrentD3DHeap.numDescriptor = max<uint32_t>(1, m_CommittedNumDescriptor);
		m_CurrentD3DHeap.DescriptorHeap = CreateD3DDescriptorHeap(g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, m_CurrentD3DHeap.numDescriptor);

	}
	void ResourceStateTracker::TransitionBarrier(const Resource& rsc, D3D12_RESOURCE_STATES state, uint32_t subResource)
	{
		if (!rsc.IsVaild()) assert("ResourceStateTracker::TransitionBarrier resource is null");

		if (m_ResourceStates.find(rsc.GetD3DPtr()) == m_ResourceStates.end())
		{
			m_PendingResourceBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
				rsc.GetD3DPtr(), state, state, subResource));
		}
		else
		{
			auto resourceState = m_ResourceStates[rsc.GetD3DPtr()];
			if (subResource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
				!resourceState.StateMap.empty())
			{
				for (auto& state_pair : resourceState.StateMap)
				{
					D3D12_RESOURCE_STATES prevState = state_pair.second;
					D3D12_RESOURCE_STATES nextState = state;

					if (prevState != nextState)
					{
						m_ResourceBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
							rsc.GetD3DPtr(), prevState, nextState, subResource));
					}
				}
			}
			else
			{
				D3D12_RESOURCE_STATES prevState = resourceState.Get(subResource);
				D3D12_RESOURCE_STATES nextState = state;
				if (prevState != nextState)
				{
					m_ResourceBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
						rsc.GetD3DPtr(), prevState, nextState, subResource));
				}
			}
		}

		m_ResourceStates[rsc.GetD3DPtr()].Set(subResource, state);
	}

	void ResourceStateTracker::UAVBarrier(const Resource& rsc)
	{
		if (rsc.IsVaild())
		{
			m_ResourceBarriers.push_back(
				CD3DX12_RESOURCE_BARRIER::UAV(rsc.GetD3DPtr()));

		}
		else
		{
			m_ResourceBarriers.push_back(
				CD3DX12_RESOURCE_BARRIER::UAV(nullptr));
		}
	}
	void ResourceStateTracker::AliasBarrier(const Resource& before_rsc, const Resource& after_rsc)
	{
		ID3D12Resource* before_d3d_rsc = (before_rsc.IsVaild()) ?
			before_rsc.GetD3DPtr() : nullptr;
		ID3D12Resource* after_d3d_rsc = (after_rsc.IsVaild()) ?
			after_rsc.GetD3DPtr() : nullptr;


		m_ResourceBarriers.push_back(
			CD3DX12_RESOURCE_BARRIER::Aliasing(before_d3d_rsc, after_d3d_rsc));
	}

	void ResourceStateTracker::FlushResourceBarrier(ID3D12GraphicsCommandList* cmdList)
	{
		if (m_ResourceBarriers.empty()) return;


		cmdList->ResourceBarrier((uint32_t)m_ResourceBarriers.size(), m_ResourceBarriers.data());
		m_ResourceBarriers.clear();
	}

	bool ResourceStateTracker::FlushPendingResourceBarrier(ID3D12GraphicsCommandList* cmdList)
	{
		if (m_PendingResourceBarriers.empty()) return false;

		vector<D3D12_RESOURCE_BARRIER> pendingBarriers;
		for (auto& pending_barrier : m_PendingResourceBarriers)
		{
			auto d3d_pResource = pending_barrier.Transition.pResource;
			if (g_ResourceRefMap.find(d3d_pResource) != g_ResourceRefMap.end())
			{
				auto resourceState = g_ResourceRefMap[d3d_pResource].state;
				uint32_t subresource = pending_barrier.Transition.Subresource;
				if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
					!resourceState.StateMap.empty())
				{
					for (auto subresourceState : resourceState.StateMap)
					{
						auto prevState = subresourceState.second;
						auto nextState = pending_barrier.Transition.StateAfter;
						if (prevState != nextState)
						{
							pending_barrier.Transition.Subresource = subresourceState.first;
							pending_barrier.Transition.StateBefore = prevState;
							pendingBarriers.push_back(pending_barrier);
						}
					}
				}
				else
				{
					auto prevState = resourceState.Get(subresource);
					auto nextState = pending_barrier.Transition.StateAfter;
					if (prevState != nextState)
					{
						pending_barrier.Transition.Subresource = subresource;
						pending_barrier.Transition.StateBefore = prevState;
						pendingBarriers.push_back(pending_barrier);
					}
				}
			}
		}
		if (pendingBarriers.empty()) return false;



		cmdList->ResourceBarrier((uint32_t)pendingBarriers.size(), pendingBarriers.data());
		m_PendingResourceBarriers.clear();

		return true;
	}
	void ResourceStateTracker::CommitResourceState()
	{
		for (auto& stateMap : m_ResourceStates)
		{
			g_ResourceRefMap[stateMap.first].state = stateMap.second;
		}
		m_ResourceStates.clear();
	}
	void ResourceStateTracker::Reset()
	{
		m_ResourceBarriers.clear();
		m_PendingResourceBarriers.clear();
		m_ResourceStates.clear();
	}
}

// Shader
namespace DX12
{
	bool Shader::Compile(const std::string& entry, const std::string& version)
	{
		if (!m_IsDirty) return true;
		m_IsDirty = false;
		string _ver_;
		switch (GetType())
		{
		case ShaderType_Vertex:
			_ver_ = "vs_";
			break;
		case ShaderType_Hull:
			_ver_ = "hs_";
			break;
		case ShaderType_Domain:
			_ver_ = "ds_";
			break;
		case ShaderType_Geometry:
			_ver_ = "gs_";
			break;
		case ShaderType_Pixel:
			_ver_ = "ps_";
			break;
		case ShaderType_Compute:
			_ver_ = "cs_";
			break;
		}
		ComPtr<ID3DBlob> error;
		HRESULT hr = D3DCompile2(
			m_SourceCode.data(),
			m_SourceCode.size(),
			m_Name.c_str(),
			nullptr,
			nullptr,
			entry.c_str(),
			(_ver_ + version).c_str(),
			0, 0, 0, nullptr, 0,
			m_Data.GetAddressOf(),
			error.GetAddressOf());

		if (FAILED(hr))
		{
			m_ErrorCode = (char*)error->GetBufferPointer();
			return false;
		}

		return true;
	}
}


// Resource
namespace DX12
{
	Resource::Resource(const Resource& copy)
	{
		m_D3DResource = copy.m_D3DResource;
		m_Name = copy.m_Name;
		IncreaseResourceRefCount(m_D3DResource.Get());
	}
	Resource::Resource(Resource&& rhs)
	{
		m_D3DResource = rhs.m_D3DResource;
		m_Name = rhs.m_Name;

		rhs.m_D3DResource = nullptr;
		rhs.m_Name = "";
	}
	Resource::~Resource()
	{
		//if (is_origin) return;
		if (!IsVaild()) return;
		DecreaseResourceRefCount(m_D3DResource.Get());
	}

	Resource& Resource::operator=(const Resource& rhs)
	{
		if (rhs.m_D3DResource == m_D3DResource) return *this;
		if (IsVaild())
		{
			DecreaseResourceRefCount(m_D3DResource.Get());
		}

		m_D3DResource = rhs.m_D3DResource;
		m_Name = rhs.m_Name;

		if (m_D3DResource)
		{
			IncreaseResourceRefCount(m_D3DResource.Get());
		}
		return *this;
	}
	Resource& Resource::operator=(Resource&& rhs)
	{
		if (IsVaild() && rhs.m_D3DResource != m_D3DResource)
		{
			DecreaseResourceRefCount(m_D3DResource.Get());
		}

		m_D3DResource = rhs.m_D3DResource;
		m_Name = rhs.m_Name;

		rhs.m_D3DResource = nullptr;
		rhs.m_Name = "";
		return *this;
	}
	void Resource::Create(const std::string& name, const D3D12_RESOURCE_DESC& desc,
		D3D12_RESOURCE_STATES initState, D3D12_CLEAR_VALUE* clearValue,
		D3D12_HEAP_PROPERTIES heapProperties, D3D12_HEAP_FLAGS heapFlags)
	{
		if (IsVaild())
		{
			DecreaseResourceRefCount(m_D3DResource.Get());
			m_D3DResource = nullptr;
		}

		g_Device->CreateCommittedResource(
			&heapProperties,
			heapFlags,
			&desc,
			initState,
			clearValue,
			IID_PPV_ARGS(m_D3DResource.GetAddressOf()));
		RegisterResource(*this, name, m_D3DResource.Get(), initState);
	}

	Texture::Texture(const Texture& copy) : Resource(copy)
	{
		
	}
	Texture::Texture(Texture&& rhs) : Resource(rhs)
	{

	}

	Texture& Texture::operator=(const Texture& rhs)
	{
		Resource::operator=(rhs);
		CreateView();
		return *this;
	}
	Texture& Texture::operator=(Texture&& rhs)
	{
		Resource::operator=(rhs);
		CreateView();
		return *this;
	}


	void Texture::Create(const std::string& name, const D3D12_RESOURCE_DESC& desc,
		D3D12_RESOURCE_STATES initState,
		D3D12_CLEAR_VALUE* clearValue,
		D3D12_HEAP_PROPERTIES heapProperties, D3D12_HEAP_FLAGS heapFlags)
	{
		Resource::Create(name, desc, initState, clearValue, heapProperties, heapFlags);

		if (m_ClearValue == nullptr) m_ClearValue = make_unique<D3D12_CLEAR_VALUE>();
		if (clearValue)
		{
			*m_ClearValue = *clearValue;
		}
		else
		{
			if (m_ClearValue) m_ClearValue = nullptr;
		}


		CreateView();
	}

	void Texture::CreateTexture(
		const std::string& name, DXGI_FORMAT format, 
		uint32_t width, uint32_t height, uint32_t miplevel, uint32_t arraySize)
	{
		Create(name, CD3DX12_RESOURCE_DESC::Tex2D(format, width, height,
			arraySize, miplevel, 1, 0));
	}

	void Texture::CreateUnordoredAccessTexture(
		const std::string& name, DXGI_FORMAT format,
		uint32_t width, uint32_t height, uint32_t miplevel, uint32_t arraySize)
	{
		Create(name, CD3DX12_RESOURCE_DESC::Tex2D(format, width, height,
			arraySize, miplevel, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));
	}

	void Texture::CreateRenderTargetTexture(
		const std::string& name, DXGI_FORMAT format, 
		uint32_t width, uint32_t height, uint32_t miplevel, uint32_t arraySize, const float4& clearColor)
	{
		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = format;
		clearValue.Color[0] = clearColor.x;
		clearValue.Color[1] = clearColor.y;
		clearValue.Color[2] = clearColor.z;
		clearValue.Color[3] = clearColor.w;


		Create(name, CD3DX12_RESOURCE_DESC::Tex2D(format, width, height,
			arraySize, miplevel, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
			D3D12_RESOURCE_STATE_COMMON, &clearValue);
	}


	void Texture::CreateDepthStencilTexture(
		const std::string& name, DXGI_FORMAT format,
		uint32_t width, uint32_t height, uint32_t miplevel, uint32_t arraySize, float clear_depth, uint8_t clear_stencil)
	{
		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = format;
		clearValue.DepthStencil.Depth = clear_depth;
		clearValue.DepthStencil.Stencil = clear_stencil;
		Create(name, CD3DX12_RESOURCE_DESC::Tex2D(format, width, height,
			arraySize, miplevel, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			D3D12_RESOURCE_STATE_COMMON, &clearValue);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetRTV(D3D12_RENDER_TARGET_VIEW_DESC* desc) const
	{
		if(!IsVaild()) return D3D12_CPU_DESCRIPTOR_HANDLE();
		auto resource_Desc = GetDesc();
		if (resource_Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) 
			assert("need flag D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET");



		size_t seed = 0;
		if (desc)
		{
			seed = hash< D3D12_RENDER_TARGET_VIEW_DESC>{}(*desc);
		}


		lock_guard<mutex> lock(m_RTVMutex);
		if (m_RTVs.find(seed) == m_RTVs.end())
		{
			auto alloc = RTVAllocate();
			g_Device->CreateRenderTargetView(GetD3DPtr(), desc, alloc.CPU());
			m_RTVs[seed] = move(alloc);
		}

		return m_RTVs[seed].CPU();


	}
	D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetSRV(D3D12_SHADER_RESOURCE_VIEW_DESC* desc) const
	{
		size_t seed = 0;
		if (desc)
		{
			seed = hash<D3D12_SHADER_RESOURCE_VIEW_DESC>{}(*desc);
		}


		lock_guard<mutex> lock(m_SRVMutex);
		if (m_SRVs.find(seed) == m_SRVs.end())
		{
			auto alloc = CSUAllocate();
			g_Device->CreateShaderResourceView(GetD3DPtr(), desc, alloc.CPU());
			m_SRVs[seed] = move(alloc);
		}
		return m_SRVs[seed].CPU();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetDSV(D3D12_DEPTH_STENCIL_VIEW_DESC* desc) const
	{
		if (!IsVaild()) return D3D12_CPU_DESCRIPTOR_HANDLE();
		auto resource_Desc = GetDesc();
		if (resource_Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
			assert("need flag D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL");


		size_t seed = 0;
		if (desc)
		{
			seed = hash<D3D12_DEPTH_STENCIL_VIEW_DESC>{}(*desc);
		}


		lock_guard<mutex> lock(m_DSVMutex);
		if (m_DSVs.find(seed) == m_DSVs.end())
		{
			auto alloc = DSVAllocate();
			g_Device->CreateDepthStencilView(GetD3DPtr(), desc, alloc.CPU());
			m_DSVs[seed] = move(alloc);
		}

		return m_DSVs[seed].CPU();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUAV(D3D12_UNORDERED_ACCESS_VIEW_DESC* desc) const
	{
		if (!IsVaild()) return D3D12_CPU_DESCRIPTOR_HANDLE();
		auto resource_Desc = GetDesc();
		if (resource_Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) 
			assert("need flag D3D12_UNORDERED_ACCESS_VIEW_DESC");


		size_t seed = 0;
		if (desc)
		{
			seed = hash<D3D12_UNORDERED_ACCESS_VIEW_DESC>{}(*desc);
		}


		lock_guard<mutex> lock(m_UAVMutex);
		if (m_UAVs.find(seed) == m_UAVs.end())
		{
			auto alloc = CSUAllocate();
			g_Device->CreateUnorderedAccessView(GetD3DPtr(), nullptr, desc, alloc.CPU());
			m_UAVs[seed] = move(alloc);
		}
		return m_UAVs[seed].CPU();
	}

	void Texture::Resize(uint32_t width, uint32_t height)
	{
		if (!IsVaild()) return;
		


		auto desc = GetDesc();
		if (desc.Width == width && desc.Height == height) return;




		desc.Width  = max<uint32_t>(1, width);
		desc.Height = max<uint32_t>(1, height);

		Create(GetName(), desc, D3D12_RESOURCE_STATE_COMMON, m_ClearValue.get());
	}

	void Texture::CreateView()
	{
		if (GetD3DPtr())
		{
			lock_guard<mutex> rtv_lock(m_RTVMutex);
			lock_guard<mutex> srv_lock(m_SRVMutex);
			lock_guard<mutex> dsv_lock(m_DSVMutex);
			lock_guard<mutex> uav_lock(m_UAVMutex);
			m_RTVs.clear();
			m_SRVs.clear();
			m_DSVs.clear();
			m_UAVs.clear();
		}
	}


	void Buffer::Create(const std::string& name, uint32_t btSize, D3D12_RESOURCE_STATES initState, D3D12_RESOURCE_FLAGS flags)
	{

		Resource::Create(
			name, CD3DX12_RESOURCE_DESC::Buffer(btSize, flags),
			initState, nullptr,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE);


	}
	void UploadBuffer::Create(const std::string& name, uint32_t btSize,
		D3D12_RESOURCE_STATES initState, D3D12_RESOURCE_FLAGS flags)
	{
		Resource::Create(
			name, CD3DX12_RESOURCE_DESC::Buffer(btSize),
			initState, nullptr,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE);
	}


	void ConstantBuffer::Create(const std::string& name, uint32_t btSize,
		D3D12_RESOURCE_STATES initState,
		D3D12_RESOURCE_FLAGS flags)
	{

		UploadBuffer::Create(name, (btSize + 255) & ~255, initState);

	}
	D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetCBV() const
	{
		return m_CBV.CPU();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE StructuredBuffer::GetSRV() const
	{
	
		return m_SRV.CPU();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE StructuredBuffer::GetUAV() const
	{
		return m_UAV.CPU();
	}

	bool RootSignature::Finalize()
	{

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc((uint32_t)m_RootParams.size(),
			m_RootParams.data(), (uint32_t)m_SamplerState.size(), m_SamplerState.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		size_t HashCode = HashState(&rootSigDesc.Flags);

		HashCode = HashState(rootSigDesc.pStaticSamplers, m_SamplerState.size(), HashCode);



		for (uint32_t param = 0; param < (uint32_t)m_RootParams.size(); ++param)
		{
			const D3D12_ROOT_PARAMETER& RootParam = rootSigDesc.pParameters[param];

			if (RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				HashCode = HashState(RootParam.DescriptorTable.pDescriptorRanges,
					RootParam.DescriptorTable.NumDescriptorRanges, HashCode);

			}
			else
				HashCode = HashState(&RootParam, 1, HashCode);
		}



		ID3D12RootSignature** RSRef = nullptr;
		bool firstCompile = false;
		{
			static mutex s_HashMapMutex;
			lock_guard<mutex> CS(s_HashMapMutex);
			auto iter = g_RootSigCahce.find(HashCode);

			// Reserve space so the next inquiry will find that someone got here first.
			if (iter == g_RootSigCahce.end())
			{
				RSRef = g_RootSigCahce[HashCode].GetAddressOf();
				firstCompile = true;
			}
			else
				RSRef = iter->second.GetAddressOf();
		}

		if (firstCompile)
		{
			m_D3D_RootSig = CreateD3DRootSignature(DXDevice::GetD3DDevice(), &rootSigDesc);
			if (m_D3D_RootSig == nullptr) assert("failed CreateRootSig");
			g_RootSigCahce[HashCode] = m_D3D_RootSig.Get();
		}
		else
		{
			while (*RSRef == nullptr)
				this_thread::yield();
			m_D3D_RootSig = *RSRef;
		}

		m_DescriptorRanges.clear();
		return true;
	}
	GraphicsPipelineState::GraphicsPipelineState()  : PipelineState()
	{
		m_Desc.SampleMask = UINT_MAX;
		m_Desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		m_Desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		m_Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		m_Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_Desc.SampleDesc.Count = 1;
		m_Desc.SampleDesc.Quality = 0;
	}
	void GraphicsPipelineState::BindRootSignature(const RootSignature& rootSig)
	{
		m_Desc.pRootSignature = rootSig.GetD3DPtr();
	}
	void GraphicsPipelineState::BindRenderTarget(RenderTarget& renderTarget)
	{
		uint32_t numRenderTarget = 0;
		for (int i = 0; i < MAX_NUM_RENDERTARGET; ++i)
		{
			if (!renderTarget[i].IsVaild()) break;

			m_Desc.RTVFormats[i] = renderTarget[i].GetDesc().Format;

			++numRenderTarget;
		}
		
		m_Desc.NumRenderTargets = numRenderTarget;
		if (renderTarget.GetDepthStencilTexture().IsVaild())
		{
			m_Desc.DSVFormat = renderTarget.GetDepthStencilTexture().GetDesc().Format;
		}
	}
	void GraphicsPipelineState::BindShader(ShaderType type, const Shader& shader)
	{
		switch (type)
		{
		case ShaderType_Vertex:
			m_Desc.VS = {
				reinterpret_cast<BYTE*>(shader.GetPtr()),
				shader.GetSize()
			};
			break;
		case ShaderType_Hull:
			m_Desc.HS = {
				reinterpret_cast<BYTE*>(shader.GetPtr()),
				shader.GetSize()
			};
			break;
		case ShaderType_Domain:
			m_Desc.DS = {
				reinterpret_cast<BYTE*>(shader.GetPtr()),
				shader.GetSize()
			};
			break;
		case ShaderType_Geometry:
			m_Desc.GS = {
				reinterpret_cast<BYTE*>(shader.GetPtr()),
				shader.GetSize()
			};
			break;
		case ShaderType_Pixel:
			m_Desc.PS = {
				reinterpret_cast<BYTE*>(shader.GetPtr()),
				shader.GetSize()
			};
			break;
		}
	}
	void GraphicsPipelineState::BindInputLayout(const InputLayout& inputLayout)
	{
		m_InputLayout = inputLayout;
		m_Desc.InputLayout = { inputLayout.m_ElementDesc.data(), (uint32_t)inputLayout.m_ElementDesc.size() };
	}
	void GraphicsPipelineState::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
	{
		m_Desc.PrimitiveTopologyType = type;
	}
	void GraphicsPipelineState::SetSampleMask(uint32_t sampleMask)
	{
		m_Desc.SampleMask = sampleMask;
	}
	void GraphicsPipelineState::SetRasterizerState(const D3D12_RASTERIZER_DESC& desc)
	{
		m_Desc.RasterizerState = desc;
	}
	void GraphicsPipelineState::SetBlendState(const D3D12_BLEND_DESC& desc)
	{
		m_Desc.BlendState = desc;
	}
	void GraphicsPipelineState::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc)
	{
		m_Desc.DepthStencilState = desc;
	}
	bool GraphicsPipelineState::Finalize()
	{
		m_Desc.InputLayout.pInputElementDescs = nullptr;
		size_t hashCode = HashState(&m_Desc);
		hashCode = HashState(m_InputLayout.m_ElementDesc.data(), m_Desc.InputLayout.NumElements, hashCode);

		m_Desc.InputLayout.pInputElementDescs = m_InputLayout.m_ElementDesc.data();

		ID3D12PipelineState** PSORef = nullptr;
		bool firstCompile = false;
		{
			static mutex s_HashMapMutex;
			lock_guard<mutex> CS(s_HashMapMutex);
			auto iter = g_GraphicsPSOCache.find(hashCode);

			if (iter == g_GraphicsPSOCache.end())
			{
				firstCompile = true;
				PSORef = g_GraphicsPSOCache[hashCode].GetAddressOf();
			}
			else
				PSORef = iter->second.GetAddressOf();
		}

		if (firstCompile)
		{
			HRESULT hr = g_Device->CreateGraphicsPipelineState(&m_Desc, IID_PPV_ARGS(m_D3D_PSO.GetAddressOf()));
			if (FAILED(hr))
			{
				return false;
			}
			g_GraphicsPSOCache[hashCode] = m_D3D_PSO;
		}
		else
		{
			while (*PSORef == nullptr)
				this_thread::yield();
			m_D3D_PSO = *PSORef;
		}
		return true;
	}

	void ComputePipelineState::BindRootSignature(const RootSignature& rootSig)
	{
		m_Desc.pRootSignature = rootSig.GetD3DPtr();
	}
	void ComputePipelineState::BindShader(const Shader& shader)
	{
		m_Desc.CS = {
			reinterpret_cast<BYTE*>(shader.GetPtr()),
			shader.GetSize()
		};
	}
	bool ComputePipelineState::Finalize()
	{
		size_t HashCode = HashState(&m_Desc);

		ID3D12PipelineState** PSORef = nullptr;
		bool firstCompile = false;
		{
			static mutex s_HashMapMutex;
			lock_guard<mutex> CS(s_HashMapMutex);
			auto iter = g_ComputePSOCache.find(HashCode);

			if (iter == g_ComputePSOCache.end())
			{
				firstCompile = true;
				PSORef = g_ComputePSOCache[HashCode].GetAddressOf();
			}
			else
				PSORef = iter->second.GetAddressOf();
		}

		if (firstCompile)
		{
			HRESULT hr = g_Device->CreateComputePipelineState(&m_Desc, IID_PPV_ARGS(&m_D3D_PSO));
			if (FAILED(hr))
			{
				return false;
				assert("failed Create Compute PSO");
			}
			g_ComputePSOCache[HashCode].Attach(m_D3D_PSO.Get());
		}
		else
		{
			while (*PSORef == nullptr)
				this_thread::yield();
			m_D3D_PSO = *PSORef;
		}
		return true;
	}


}

// DXModuleSystem
namespace DX12
{
	static string g_ModuleWorkDir;
	static string g_ModuleSDDefinedHeaderFile;
	static unordered_map<string, string> g_SDataMap;
	static unordered_map<string, string> g_CBDataMap;

	void replaceAll(string& dest, const string& pattern, const string& replace)
	{
		size_t pos = 0;

		while (pos != string::npos)
		{
			pos = dest.find(pattern, pos);
			if (pos == string::npos) break;
			dest.replace(dest.begin() + pos, dest.begin() + pos + pattern.length(),
				replace);
			pos += replace.length();

		}
	}

	void DXModuleSystem::Init(const std::string& workdirPath, const std::string& sdDefinedheadfilePath)
	{
		g_ModuleWorkDir = workdirPath;
		g_ModuleSDDefinedHeaderFile = sdDefinedheadfilePath;
		LoadShaderDataDefined();

	}

	RendererModule* DXModuleSystem::CreateRenderer(const std::string& name)
	{
		return nullptr;
	}
	ComputeShaderModule* DXModuleSystem::CreateComputeShaderModule(const std::string& name)
	{
		return nullptr;
	}
	RendererModule* DXModuleSystem::FindRenderer(const std::string& name)
	{
		return nullptr;
	}
	ComputeShaderModule* DXModuleSystem::FindComputeShaderModule(const std::string& name)
	{
		return nullptr;
	}
	void DXModuleSystem::LoadShaderDataDefined()
	{
		/*
// Çì´õ ÆÄÀÏÀ» ±â¹ÝÇÑGlobalSDB »ý¼º */
		ifstream fin(g_ModuleSDDefinedHeaderFile);
		if (!fin.is_open())
		{
			assert("none exist sdDefinedHeaderFile");
		}
		fin.seekg(0, ios::end);
		size_t endPos = fin.tellg();
		fin.seekg(0, ios::beg);

		string definedCode;
		definedCode.resize(endPos);
		fin.read(&definedCode[0], endPos);




		// ¼ÎÀÌ´õ µ¥ÀÌÅÍ ±¸Á¶Ã¼ ¹× »ó¼ö µ¥ÀÌÅÍ ÄÚµå ÃßÃâ

		size_t struct_define_start_pos = definedCode.find("__SD_DEFINES_START__") + strlen("__SD_DEFINES_START__");
		size_t struct_define_end_pos = definedCode.find("__SD_DEFINES_END__");

		size_t cb_define_start_pos = definedCode.find("__SCB_DEFINES_START__") + strlen("__SCB_DEFINES_START__");
		size_t cb_ddfine_end_pos = definedCode.find("__SCB_DEFINES_END__");


		//
		string struct_define_code = definedCode.substr(struct_define_start_pos, struct_define_end_pos - struct_define_start_pos);
		string cb_define_code = definedCode.substr(cb_define_start_pos, cb_ddfine_end_pos - cb_define_start_pos);

		replaceAll(struct_define_code, "\t\t", "");
		replaceAll(cb_define_code, "\t\t", "");
		//
		fin.close();


		size_t sd_code_pos = 0;
		size_t sd_struct_len = strlen("struct ");
		while ((sd_code_pos != string::npos))
		{
			sd_code_pos = struct_define_code.find("struct", sd_code_pos);
			if (sd_code_pos == string::npos)
				break;
			size_t start = sd_code_pos;
			size_t end = struct_define_code.find("};", start) + 2;

			size_t sdName_start = start + sd_struct_len;
			size_t sdName_end = struct_define_code.find("{", sdName_start);


			string sdName = struct_define_code.substr(sdName_start, sdName_end - sdName_start);
			replaceAll(sdName, "\n", "");
			string sdContents = struct_define_code.substr(start, end - start);


			g_SDataMap[sdName] = sdContents;
			sd_code_pos = end;
		}

		// »ó¼ö¹öÆÛ±¸Á¶Ã¼ µ¥ÀÌÅÍ¸Ê »ý¼º
		size_t cb_code_pos = 0;
		size_t scb_macro_len = strlen("__SCB__ ");
		while ((cb_code_pos != string::npos))
		{
			cb_code_pos = cb_define_code.find("__SCB__", cb_code_pos);
			if (cb_code_pos == string::npos)
				break;

			size_t start = cb_code_pos;
			size_t end = cb_define_code.find("};", start) + 2;

			size_t cbName_start = start + scb_macro_len;
			size_t cbName_end = cb_define_code.find("{", cbName_start);

			string cbName = cb_define_code.substr(cbName_start, cbName_end - cbName_start);
			replaceAll(cbName, "\n", "");
			string cbContents = cb_define_code.substr(start, end - start);
			replaceAll(cbContents, "__SCB__", "cbuffer");


			g_CBDataMap[cbName] = cbContents;
			cb_code_pos = end;
		}
	}
	void DXModuleSystem::LoadRenderModule()
	{

	}
}

// DXModule Class
namespace DX12
{
	void RendererModuleInitDesc::SignConstantBuffer(
		uint32_t registernum, uint32_t registerspace, const string& struct_name)
	{
		if (signInfo.find(struct_name) != signInfo.end()) return;

		SignInfo info;
		info.type = __CBuffer__;
		info.register_num = registernum;
		info.register_space = registerspace;
		info.struct_name = struct_name;
		signInfo.emplace(struct_name, info);
	}
	void RendererModuleInitDesc::SignStructuredBuffer(
		const string& name, uint32_t registernum, uint32_t registerspace, const string& struct_name)
	{
		if (signInfo.find(name) != signInfo.end()) return;

		SignInfo info;
		info.type = __StructuredBuffer__;
		info.register_num = registernum;
		info.register_space = registerspace;
		info.struct_name = struct_name;
		signInfo.emplace(name, info);
	}
	void RendererModuleInitDesc::SignRWStructuredBuffer(
		const string& name, uint32_t registernum, uint32_t registerspace, const string& struct_name)
	{
		if (signInfo.find(name) != signInfo.end()) return;

		SignInfo info;
		info.type = __RWStructuredBuffer__;
		info.register_num = registernum;
		info.register_space = registerspace;
		info.struct_name = struct_name;
		signInfo.emplace(name, info);
	}
	void RendererModuleInitDesc::SignTexture2D(
		const string& name, uint32_t numDescriptor, uint32_t registernum, uint32_t registerspace)
	{
		if (signInfo.find(name) != signInfo.end()) return;

		SignInfo info;
		info.type = __Texture2D__;
		info.numDescriptor = numDescriptor;
		info.register_num = registernum;
		info.register_space = registerspace;
		signInfo.emplace(name, info);
	}
	void RendererModuleInitDesc::SignRWTexture2D(
		const string& name, uint32_t numDescriptor, uint32_t registernum, uint32_t registerspace)
	{
		if (signInfo.find(name) != signInfo.end()) return;

		SignInfo info;
		info.type = __RWTexture2D__;
		info.numDescriptor = numDescriptor;
		info.register_num = registernum;
		info.register_space = registerspace;
		signInfo.emplace(name, info);
	}
	void RendererModuleInitDesc::SignTextureCube(
		const string& name, uint32_t numDescriptor, uint32_t registernum, uint32_t registerspace)
	{
		if (signInfo.find(name) != signInfo.end()) return;

		SignInfo info;
		info.type = __TextureCube__;
		info.numDescriptor = numDescriptor;
		info.register_num = registernum;
		info.register_space = registerspace;
		signInfo.emplace(name, info);

	}

	void RendererModuleInitDesc::SignStaticSampler(const std::string& name, const D3D12_STATIC_SAMPLER_DESC& desc)
	{
		samplerInfo[name] = desc;
	}

	void RendererModuleInitDesc::SetEntryObject(const std::string& name, Input_Vertex_Types type, const std::string& flag)
	{
		if (entryInfo.find(name) != entryInfo.end()) return;


		EntryInfo info;
		info.binding_dataName = name;
		info.type = type;
		info.flag = flag;

		entryInfo.emplace(flag, info);

	}

	void RendererModuleInitDesc::AddRenderTarget(const std::string& name, DXGI_FORMAT format)
	{
		if (rtInfo.size() >= MAX_NUM_RENDERTARGET) return;

		RTInfo info;
		info.name = name;
		info.format = format;
		
		switch (format)
		{
		case DXGI_FORMAT_R32_FLOAT: case DXGI_FORMAT_R16_UNORM: case DXGI_FORMAT_R8_UNORM:  
		case DXGI_FORMAT_R16_SNORM: case DXGI_FORMAT_R8_SNORM:
			info.data_type = "float";
			break;
		case DXGI_FORMAT_R32G32_FLOAT: case DXGI_FORMAT_R16G16_UNORM: case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R16G16_SNORM: case DXGI_FORMAT_R8G8_SNORM:
			info.data_type = "float2";
			break;
		case DXGI_FORMAT_R32G32B32_FLOAT:
			info.data_type = "float3";
			break;
		case DXGI_FORMAT_R32G32B32A32_FLOAT: case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM:     case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
			info.data_type = "float4";
			break;
		default:
			assert("RendererModuleInitDesc::AddRenderTarget not support format");
		}


		rtInfo.push_back(info);


	}



	bool RendererModule::SubmitDescAndCreateRootSigFile(const RendererModuleInitDesc& desc)
	{

		// ÆÄÀÏ »ý¼º

		// Renderer ÀÌ¸§ Æú´õ »ý¼º
		// 
		fs::path fp(g_ModuleWorkDir + GetName());
		if (!fs::exists(fp) || !fs::is_directory(fp))
		{
			if (!fs::create_directory(fp))
			{
				return false;
			}
		}
		
		// ¸ðµâ »ý¼º
		// Á¦ÀÏ ¹Û¿¡´Â .rm ÆÄÀÏ »ý¼º( ·»´õ·¯ ¸ðµâ Á¤º¸)
		{
			ofstream fout(fp.string() + "/" + GetName() + __SHADER_RENDERER_MODULE_FORMAT__);
			if (!fout.is_open()) return false;


			// µ¥ÀÌÅÍ ¾ç½Ä




			fout.close();
		}

		// hlsl ÆÄÀÏ »ý¼º
		{
			fs::path shader_fp(g_ModuleWorkDir + GetName() + "/Shader");
			if (!fs::exists(shader_fp) || !fs::is_directory(shader_fp))
			{
				if (!fs::create_directory(shader_fp))
				{
					return false;
				}
			}

			string path = shader_fp.string() + "/Common.hlsl";
			fs::path hlsl_fp(path);
			// Common ·çÆ® ¼­¸í hlsl ÆÄÀÏ »ý¼º
			if(!fs::exists(hlsl_fp))
			{
				ofstream fout(path);
				if (!fout.is_open()) return false;

				// ±¸Á¶Ã¼ ¼±¾ð
				for (auto& sign_pair : desc.signInfo)
				{

					string name = sign_pair.first;
					auto signInfo = sign_pair.second;
					if (signInfo.type != __StructuredBuffer__ ||
						g_SDataMap.find(signInfo.struct_name) == g_SDataMap.end())
					{
						continue;
					}



					fout << g_SDataMap[signInfo.struct_name] << endl;
				}

				// µ¥ÀÌÅÍ ¹ÙÀÎµù ¼±¾ð
				for (auto& sign_pair : desc.signInfo)
				{
					string name = sign_pair.first;
					auto signInfo = sign_pair.second;
					string registerStr = " : register(____registernumber____, ____registerspace____)";
					switch (signInfo.type)
					{
					case __CBuffer__:
					{
						if (g_CBDataMap.find(signInfo.struct_name) == g_CBDataMap.end())
							return false;
						string contents = g_CBDataMap[signInfo.struct_name];
						replaceAll(registerStr, "____registernumber____", "b" + to_string(signInfo.register_num));
						replaceAll(registerStr, "____registerspace____", "space" + to_string(signInfo.register_space));
						replaceAll(contents, signInfo.struct_name, signInfo.struct_name + registerStr);

						fout << contents << endl;
					}
					break;
					case __StructuredBuffer__:
					{
						string contents = "StructuredBuffer<" + signInfo.struct_name + "> " + name;
						replaceAll(registerStr, "____registernumber____", "t" + to_string(signInfo.register_num));
						replaceAll(registerStr, "____registerspace____", "space" + to_string(signInfo.register_space));

						fout << contents + registerStr + ";" << endl;
					}

					break;
					case __RWStructuredBuffer__:
					{
						string contents = "RWStructuredBuffer<" + signInfo.struct_name + "> " + name;
						replaceAll(registerStr, "____registernumber____", "u" + to_string(signInfo.register_num));
						replaceAll(registerStr, "____registerspace____", "space" + to_string(signInfo.register_space));


						fout << contents + registerStr + ";" << endl;
					}
					break;
					case __Texture2D__:
					{
						string contents = (signInfo.numDescriptor == 1) ?
							"Texture2D " + name :
							"Texture2D " + name + "[" + to_string(signInfo.numDescriptor) + "]";

						replaceAll(registerStr, "____registernumber____", "t" + to_string(signInfo.register_num));
						replaceAll(registerStr, "____registerspace____", "space" + to_string(signInfo.register_space));


						fout << contents + registerStr + ";" << endl;
					}
					break;
					case __RWTexture2D__:
					{
						string contents = (signInfo.numDescriptor == 1) ?
							"RWTexture2D " + name :
							"RWTexture2D " + name + "[" + to_string(signInfo.numDescriptor) + "]";
						replaceAll(registerStr, "____registernumber____", "u" + to_string(signInfo.register_num));
						replaceAll(registerStr, "____registerspace____", "space" + to_string(signInfo.register_space));


						fout << contents + registerStr + ";" << endl;
					}
					break;
					case __TextureCube__:
					{
						string contents = (signInfo.numDescriptor == 1) ?
							"TextureCube " + name :
							"TextureCube " + name + "[" + to_string(signInfo.numDescriptor) + "]";
						replaceAll(registerStr, "____registernumber____", "t" + to_string(signInfo.register_num));
						replaceAll(registerStr, "____registerspace____", "space" + to_string(signInfo.register_space));


						fout << contents + registerStr + ";" << endl;
					}
					break;
					}
				}

				for (auto& sampler_pair : desc.samplerInfo)
				{
					string contents = "SamplerState " + sampler_pair.first +
						" : register(s" + to_string(sampler_pair.second.ShaderRegister) + ", space" +
						to_string(sampler_pair.second.RegisterSpace) + ");";

					fout << contents << endl;
				}


				fout.close();
			}



			path = shader_fp.string() + "/Entry.hlsl";
			hlsl_fp = fs::path(path);
			// Çìµå hlsl ÆÄÀÏ ¸¸µé±â
			if(!fs::exists(hlsl_fp))
			{
				ofstream fout(path);
				if (!fout.is_open()) return false;

				string shaderCode = "";
				string include = R"(#include "Common.hlsl")";
				include += "\n";
				string flagPattern = "__FLAG__";
				string inputLayoutPattern = "__INPUTELEMENT__";
				string renderTargetPattern = "__RENDERTARGET__";
				string vsCodePattern = "__CODEBYFLAGS__";

				string inputLayout_templateCode = R"(
#if defined(__FLAG__)
	__INPUTELEMENT__
#endif
)";
				string out_templateCode = R"(
struct VS_OUT
{
	float4 posH : SV_POSITION;
	// TODO()
};

struct PS_OUT
{
__RENDERTARGET__
};

)";

				// entry ÆÄÀÏ »ý¼º
				string maincodetemplate = R"(
VS_OUT vs(VS_IN vin, uint insID : SV_InstanceID)
{
	VS_OUT vout;

__CODEBYFLAGS__

	return vout;
}

PS_OUT ps(VS_OUT pin)
{
	PS_OUT pout;

	// TODO(ps process)

	return pout;
}
)" ;

				string vsByFlagCode;
				// include
				shaderCode += include + "\n";

				string input_layout_code = "struct VS_IN \n{ \n";
				// input Layout
				for (auto& entry_pair : desc.entryInfo)
				{
					string flag = entry_pair.first;
					auto entryInfo = entry_pair.second;

					string inputLayoutStr = inputLayout_templateCode;

					string replace;

					switch (entryInfo.type)
					{
					case Input_Vertex_Type_Static:
						replace = R"(
	float3 posL     : POSITION;
	float3 normalL  : NORMAL;
	float3 tangentL : TANGENT;
	float2 tex      : TEXCOORD;
)";
						break;
					case Input_Vertex_Type_Skinned:
						replace = R"(
	float3 posL     : POSITION;
	float3 normalL  : NORMAL;
	float3 tangentL : TANGENT;
	float2 tex      : TEXCOORD;
	float4 boneWeight : BONDWEIGHT;
	int4   bondID : BONDID;
)";
						break;
					case Input_Vertex_Type_Paper:
						replace = R"(
	float3 posL     : POSITION;
	float4 color    : COLOR;
	float2 tex      : TEXCOORD;
)";
						break;
					}
					replaceAll(inputLayoutStr, flagPattern, flag);
					replaceAll(inputLayoutStr, inputLayoutPattern, replace);
					
					input_layout_code += inputLayoutStr;

					vsByFlagCode += "#if defined(" + flag + ")\n\t// TODO \n#endif\n";
				}
				input_layout_code += "\n};";
				shaderCode += input_layout_code;
				
				// renderTarget
				{
					string replace;
					string rtCode = out_templateCode;
					for (int i = 0; i < (int)desc.rtInfo.size(); ++i)
					{

						auto& rtInfo = desc.rtInfo[i];
						replace += rtInfo.data_type + " " + rtInfo.name + " : SV_Target" + to_string(i) + ";\n";
					}
					replaceAll(rtCode, renderTargetPattern, replace);
					shaderCode += rtCode;
				}

				// vs by flag 
				{
					string maincode = maincodetemplate;
					replaceAll(maincode, vsCodePattern, vsByFlagCode);
					shaderCode += maincode;
				}
				fout << shaderCode << endl;
				fout.close();
			}



			path = shader_fp.string() + "/Final.hlsl";
			hlsl_fp = fs::path(path);
			if(!fs::exists(hlsl_fp))
			{
				string path = shader_fp.string() + "/Final.hlsl";
				ofstream fout(path);
				if (!fout.is_open()) return false;

				string shaderCode = R"(


struct VS_OUT
{
	float4 posH : SV_POSITION;
	float2 tex  : TEXCOORD;
};


VS_OUT vs(uint vertexID : SV_VertexID)
{
	VS_OUT vout;
	vout.tex = float2(vertexID % 2, (vertexID % 4) >> 1);
	vout.posH = float4((vout.tex.x - 0.5f) * 2, -(vout.tex.y - 0.5f) * 2, 0 , 1);
	return vout;
}
float4 ps(VS_OUT pout) : SV_Target
{
	// TODO
	return float4(1.0f,1.0f,1.0f,1.0f);
}

)";
				fout << shaderCode << endl;
				fout.close();
			}
			// Å×ÀÏ hlsl ÆÄÀÏ ¸¸µé±â
		}


		// ±×·¡ÇÈ ¸ðµâ
		{
			fs::path module_fp(g_ModuleWorkDir + GetName() + "/Module");
			if (!fs::exists(module_fp) || !fs::is_directory(module_fp))
			{
				if (!fs::create_directory(module_fp))
				{
					return false;
				}
			}
			string path = module_fp.string() + "/Entry" + __SHADER_GRAPHICS_MODULE_FORMAT__;
			fs::path gsm_fp = fs::path(path);
			// head Module
			if(!fs::exists(gsm_fp))
			{
				
				ofstream fout(path);
				if (!fout.is_open()) return false;


				// entry ÆÄÀÏ »ý¼º


				fout.close();

			}
			path = module_fp.string() + "/Final" + __SHADER_GRAPHICS_MODULE_FORMAT__;
			gsm_fp = fs::path(path);
			if (!fs::exists(gsm_fp))
			// tail Module
			{
				ofstream fout(path);
				if (!fout.is_open()) return false;


				// tail ÆÄÀÏ »ý¼º


				fout.close();

			}
		}
	
		return false;
	}

	bool RendererModule::Execute(GraphicsCommandKeyPtr cmdKey)
	{
		return false;
	}
	GraphicsShaderModule* RendererModule::CreateNodeModule(const std::string& name)
	{
		if (m_IDByNames.find(name) != m_IDByNames.end()) return nullptr;
		auto shaderModule = make_unique<GraphicsShaderModule>();
		shaderModule->m_Name = name;

		auto result = shaderModule.get();

		m_GSModuleByID[m_IDOffset] = move(shaderModule);
		m_IDByNames[name] = m_IDOffset++;

		return result;
	}


	RendererModule::RendererModule()
	{
		m_GSModuleByID[GetHeadID()] = make_unique<GraphicsShaderModule>();
		m_GSModuleByID[GetTailID()] = make_unique<GraphicsShaderModule>();

	}

	void GraphicsShaderModule::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc)
	{
		m_OriginPSO.SetDepthStencilState(desc);
	}
	void GraphicsShaderModule::SetBlendState(const D3D12_BLEND_DESC& desc)
	{
		m_OriginPSO.SetBlendState(desc);
	}
	void GraphicsShaderModule::SetRasterizerState(const D3D12_RASTERIZER_DESC& desc)
	{
		m_OriginPSO.SetRasterizerState(desc);
	}
	void GraphicsShaderModule::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
	{
		m_OriginPSO.SetPrimitiveTopologyType(type);
	}
	void GraphicsShaderModule::SetSampleMask(uint32_t mask)
	{
		m_OriginPSO.SetSampleMask(mask);
	}
}

namespace DX12
{
	std::string g_grm_signInfo_template = R"(
	__name__ = 
	{
		__type__
		__register__
		__numDescriptor__
		__struct_name__
	}
)";
	std::string g_grm_entryInfo_template = R"(
	__flag__ = 
	{
		__struct_name__
		__input_vertex_type__
	}
)";

	std::string g_grm_moduleList_template = R"(
	Entry = __entry_path__,
	Final = __final_path__,
	Node = 
	{
		__node__
	}
)";
	std::string g_grm_connect_template = R"(
	__name__ = 
	{
		__connect__
	}
)";



	std::ofstream& operator<<(std::ofstream& fout, const RendererModuleData& data)
	{
		

		return fout;
	}
	std::ifstream& operator>>(std::ifstream& fin, RendererModuleData& data)
	{
		return fin;
	}
	std::ofstream& operator<<(std::ofstream& fout, const GraphicsShaderModuleData& data)
	{
		return fout;
	}
	std::ifstream& operator>>(std::ifstream& fin, GraphicsShaderModuleData& data)
	{
		return fin;
	}
}
// Helper
namespace DX12
{
	Microsoft::WRL::ComPtr<IDXGIFactory4> CreateDXGIFactory()
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
		UINT32 dxgiFactoryFlags = 0;

#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
		assert(SUCCEEDED(hr) && "failed DebugInterface");

		debugInterface->EnableDebugLayer();

		Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
		{
			dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
#endif
		hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory.GetAddressOf()));
		assert(SUCCEEDED(hr) && "failed Create DXGIFactory");


		return factory;
	}
	Microsoft::WRL::ComPtr<ID3D12Device>  CreateD3DDevice(
		Microsoft::WRL::ComPtr<IDXGIFactory4> factory,
		bool is_UseWrapDevice,
		DXGI_ADAPTER_DESC1* OutadapterDesc)
	{
		HRESULT hr = S_OK;
		if (is_UseWrapDevice)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter>  warpAdapter;
			Microsoft::WRL::ComPtr<ID3D12Device>  pDevice;
			hr = factory->EnumWarpAdapter(IID_PPV_ARGS(warpAdapter.GetAddressOf()));
			assert(SUCCEEDED(hr) && "failed Create warpAdapter");

			hr = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(pDevice.GetAddressOf()));
			assert(SUCCEEDED(hr) && "failed Create D3D12Device");


			return pDevice;
		}
		else
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;
			Microsoft::WRL::ComPtr<ID3D12Device>  pDevice;

			DXGI_ADAPTER_DESC1 adapterDesc = {};
			SIZE_T maxSize = 0;

			for (uint32_t Idx = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(Idx, &pAdapter); ++Idx)
			{
				DXGI_ADAPTER_DESC1 desc;
				pAdapter->GetDesc1(&desc);
				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				if (desc.DedicatedVideoMemory > maxSize &&
					SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))))
				{
					pAdapter->GetDesc1(&desc);
					maxSize = desc.DedicatedVideoMemory;
					adapterDesc = desc;
				}
			}

			if (OutadapterDesc && maxSize > 0)
			{
				*OutadapterDesc = adapterDesc;
			}
			else if (maxSize == 0)
			{
				return nullptr;
			}

			return pDevice;
		}
	}
	Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateDXGISwapChain(
		HWND hWnd,
		Microsoft::WRL::ComPtr<IDXGIFactory4> factory,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQue,
		DXGI_FORMAT format,
		uint32_t width,
		uint32_t height,
		uint32_t bufferCount)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain4;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = format;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc = { 1, 0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = bufferCount;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = 0; //  CheckTearingSupport(factory) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		hr = factory->CreateSwapChainForHwnd(cmdQue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1);
		assert(SUCCEEDED(hr) && "failed Create IDXGISwapChain4");
		hr = factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
		assert(SUCCEEDED(hr) && "failed MakeWindowAssociation");
		hr = swapChain1.As(&dxgiSwapChain4);
		assert(SUCCEEDED(hr) && "failed SwapChain1 As SwapChain4");
		return dxgiSwapChain4;
	}



	Microsoft::WRL::ComPtr<ID3D12CommandQueue> CreateD3DCommandQueue(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		D3D12_COMMAND_LIST_TYPE type)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue));
		assert(SUCCEEDED(hr) && "failed Create ID3D12CommandQueue");
		return d3d12CommandQueue;

	}


	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CreateD3DCommandList(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator,
		D3D12_COMMAND_LIST_TYPE type)
	{

		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
		hr = device->CreateCommandList(0, type, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
		assert(SUCCEEDED(hr) && "failed Create ID3D12GraphicsCommandList");
		return commandList;
	}


	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateD3DCommandAllocator(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		D3D12_COMMAND_LIST_TYPE type)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));


		assert(SUCCEEDED(hr) && "failed Create ID3D12CommandAllocator");
		return commandAllocator;
	}



	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateD3DDescriptorHeap(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		D3D12_DESCRIPTOR_HEAP_FLAGS flag,
		uint32_t numDescriptor)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = numDescriptor;
		desc.Type = type;
		desc.Flags = flag;

		hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));
		hr = device->GetDeviceRemovedReason();
		assert(SUCCEEDED(hr) && "failed Create ID3D12DescriptorHeap");


		return descriptorHeap;

	}
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateD3DRootSignature(Microsoft::WRL::ComPtr<ID3D12Device> device, CD3DX12_ROOT_SIGNATURE_DESC* DESC)
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig;

		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		HRESULT hr = D3D12SerializeRootSignature(
			DESC,
			D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(),
			errorBlob.GetAddressOf());


		hr = device->CreateRootSignature(
			0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(rootSig.GetAddressOf()));

		return rootSig;
	}

}