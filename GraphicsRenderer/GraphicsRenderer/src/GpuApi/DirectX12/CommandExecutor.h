#pragma once


#include"Dx12Include.h"
#include"Dx12Helper.h"
#include<queue>
#include<map>
#include<mutex>

namespace GR
{
	namespace Dx12
	{
		class Commander;
		class GraphicsCommander;
		class ComputeCommander;
		class CopyCommander;
		class Fence;
		class CommandExecutor
		{
			typedef std::queue<std::shared_ptr<Commander>>           CommanderQueue;
			typedef std::map<Commander*, std::shared_ptr<Commander>> CommanderPool;
		private:
			std::mutex m_CommanderMutex;
			std::shared_ptr<Fence> m_Fence;
			CommanderPool  m_CommanderPool;
			CommanderQueue m_CommanderQueue;
			ComPtr<ID3D12CommandQueue> m_CommandQueue;
			D3D12_COMMAND_LIST_TYPE m_CmdType;

			ComPtr<ID3D12Device> m_Device;
		public:
			ID3D12CommandQueue* GetD3DCommandQueue() {
				return m_CommandQueue.Get();
			}
			CommandExecutor(ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);
		public:
			Commander* GetCommander();
		public:
			uint64_t Signal();
			void WaitForFenceValue(uint64_t fenceValue);
			void Flush();
		public:
			void ExcuteCommandList(Commander* commander);
			void ExcuteCommandLists(uint32_t numCmdList, Commander** commanders);
		};

		class CommandExecutorManager
		{
		private:
			std::shared_ptr<CommandExecutor>  m_GraphicsCommandExecutor;
			std::shared_ptr<CommandExecutor>  m_ComputeCommandExecutor;
			std::shared_ptr<CommandExecutor>  m_CopyCommandExecutor;
		public:
			CommandExecutorManager() = default;
			void Initialize(ComPtr<ID3D12Device> device);
			CommandExecutor* GetCommandExecutor(int quetype);
			CommandExecutor* GetGraphicsCommandExecutor() {
				return m_GraphicsCommandExecutor.get();
			}
			CommandExecutor*  GetComputeCommandExecutor() {
				return m_ComputeCommandExecutor.get();
			}
			CommandExecutor*     GetCopyCommandExecutor() {
				return m_CopyCommandExecutor.get();
			}
		};

	}
}