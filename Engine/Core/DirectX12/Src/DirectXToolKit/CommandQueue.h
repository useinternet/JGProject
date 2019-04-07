#pragma once
#include"DirectXCore.h"

namespace Dx12
{
	class CommandList;
	class Fence;
	class CommandQueue
	{
		typedef std::queue<std::shared_ptr<CommandList>> CommandListQueue;
		typedef std::map<CommandList*, std::shared_ptr<CommandList>> CommandListPool;
	private:
		ComPtr<ID3D12CommandQueue> m_D3D_CmdQeueue;
		std::shared_ptr<Fence>     m_Fence;
		D3D12_COMMAND_LIST_TYPE    m_CmdType;
		CommandListQueue m_PendingCommandList;
		CommandListPool  m_CommandListPool;
	public:
		CommandQueue(D3D12_COMMAND_LIST_TYPE type);
		ID3D12CommandQueue* Get() const;
	public:
		CommandList* GetCommandList();
	public:
		uint64_t Signal();
		void WaitForFenceValue(uint64_t fenceValue);
		void Flush();
	public:
		void ExcuteCommandList(CommandList* cmdList);
		void ExcuteCommandLists(uint32_t numCmdList, CommandList** cmdLists);
	};
}