#pragma once
#include "DxObject.h"


namespace RE
{
	class Fence;
	class CommandList;

	class CommandQueue : public DxObject
	{
	public:
		CommandQueue(D3D12_COMMAND_LIST_TYPE type);
		uint64_t Signal();
		void WaitForFenceValue(uint64_t fenceValue);
		void Flush();

		void ExcuteCommandList(const std::vector<CommandList*> cmdLists);

		CommandList* GetCommandList();


		ID3D12CommandQueue* GetD3DCommandQueue() const {
			return m_D3D_CmdQueue.Get();
		}
	private:
		using CommandListPool = std::map<CommandList*, std::shared_ptr<CommandList>>;
		using CommandListQueue = std::map<uint32_t, std::queue<std::shared_ptr<CommandList>>>;

		std::mutex m_Mutex;
		std::shared_ptr<Fence> m_Fence;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_D3D_CmdQueue;
		D3D12_COMMAND_LIST_TYPE m_Type;
		
		CommandListPool  m_CmdListPool;
		CommandListQueue m_PendingCmdList;





	};
}