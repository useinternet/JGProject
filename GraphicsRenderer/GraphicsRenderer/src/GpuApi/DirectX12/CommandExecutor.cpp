#include"pch.h"
#include"CommandExecutor.h"
#include"Commander.h"
#include"Fence.h"
#include"GraphicsDevice.h"
using namespace std;

namespace GR
{
	namespace Dx12
	{
		CommandExecutor::CommandExecutor(ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type) 
		{
			m_CmdType = type;
			m_Device = device;
			m_CommandQueue = CreateD3DCommandQueue(device, type);
			switch (type)
			{
			case D3D12_COMMAND_LIST_TYPE_DIRECT:
				m_CommandQueue->SetName(L"Direct Command Queue");
				break;
			case D3D12_COMMAND_LIST_TYPE_COMPUTE:
				m_CommandQueue->SetName(L"Compute Command Queue");
				break;
			case D3D12_COMMAND_LIST_TYPE_COPY:
				m_CommandQueue->SetName(L"Copy Command Queue");
				break;
			}
			m_Fence = std::make_unique<Fence>();
			m_Fence->Initialize(device);
		}
		Commander* CommandExecutor::GetCommander() {

			std::lock_guard<mutex> lock(m_CommanderMutex);

			Commander* result = nullptr;
			std::shared_ptr<Commander> commander;
			if (m_CommanderQueue.empty())
			{
				switch (m_CmdType)
				{
				case D3D12_COMMAND_LIST_TYPE_DIRECT:
					commander = make_unique<GraphicsCommander>(m_Device);
					break;
				case D3D12_COMMAND_LIST_TYPE_COMPUTE:
					commander = make_unique<ComputeCommander>(m_Device);
					break;
				case D3D12_COMMAND_LIST_TYPE_COPY:
					commander = make_unique<CopyCommander>(m_Device);
					break;
				}
				result = commander.get();

				m_CommanderPool[commander.get()] = move(commander);
			}
			else
			{
				result = m_CommanderQueue.front().get();
				result->Reset();
				m_CommanderPool[result] = m_CommanderQueue.front();
				m_CommanderQueue.pop();
			}

			return result;
		}
		uint64_t CommandExecutor::Signal() {
			m_Fence->IncreaseValue();
			m_CommandQueue->Signal(m_Fence->Get(), m_Fence->GetValue());
			return m_Fence->GetValue();
		}
		void CommandExecutor::WaitForFenceValue(uint64_t fenceValue) {
			m_Fence->WaitForFenceValue(fenceValue);
		}
		void CommandExecutor::Flush() {
			uint64_t value = Signal();
			WaitForFenceValue(value);
		}
		void CommandExecutor::ExcuteCommandList(Commander* commander)
		{
			ID3D12CommandList* d3d_cmdLists[] = { commander->GetD3DCommandList() };
			m_CommandQueue->ExecuteCommandLists(1, d3d_cmdLists);
			
			m_CommanderQueue.push(m_CommanderPool[commander]);
			m_CommanderPool.erase(commander);
		}
		void CommandExecutor::ExcuteCommandLists(uint32_t numCmdList, Commander** commanders) 
		{
			std::vector<ID3D12CommandList*> d3d_cmdLists(numCmdList);
			for (uint32_t i = 0; i < numCmdList; ++i)
			{
				d3d_cmdLists[i] = commanders[i]->GetD3DCommandList();

				m_CommanderQueue.push(m_CommanderPool[commanders[i]]);
				m_CommanderPool.erase(commanders[i]);
			}
			m_CommandQueue->ExecuteCommandLists(numCmdList, d3d_cmdLists.data());
		}

		void CommandExecutorManager::Initialize(ComPtr<ID3D12Device> device)
		{
			m_GraphicsCommandExecutor = std::make_shared<CommandExecutor>(device,
				D3D12_COMMAND_LIST_TYPE_DIRECT);
			m_ComputeCommandExecutor = std::make_shared<CommandExecutor>(device,
				D3D12_COMMAND_LIST_TYPE_COMPUTE);
			m_CopyCommandExecutor = std::make_shared<CommandExecutor>(device,
				D3D12_COMMAND_LIST_TYPE_COPY);
		}

		CommandExecutor* CommandExecutorManager::GetCommandExecutor(int quetype)
		{
			switch (quetype)
			{
			case GraphicsDevice::GRAPHICS:
				return GetGraphicsCommandExecutor();
			case GraphicsDevice::COMPUTE:
				return GetComputeCommandExecutor();
			case GraphicsDevice::COPY:
				return GetCopyCommandExecutor();
			default:
				return nullptr;
			}
		}
	}
}