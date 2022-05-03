#pragma once



#include "JGCore.h"
#include "DirectX12Helper.h"



namespace JG
{
	class CommandList;
	class Fence;
	class CommandQueue
	{
	private:
		ComPtr<ID3D12CommandQueue> mD3DCmdQueue;
		D3D12_COMMAND_LIST_TYPE    mD3DType;
		UniquePtr<Fence> mFence;
		List<u64> mFenceValue;
		std::mutex mMutex;
		bool mIsLock = false;
		std::atomic_bool mIsCommandListExcute = false;
		Dictionary<u64, SortedDictionary<u64, SharedPtr<CommandList>>> mExcuteCmdLists;
		Dictionary<u64, SortedDictionary<u64, SharedPtr<CommandList>>> mExcutePendingCmdLists;
	public:
		CommandQueue(u64 bufferCount, D3D12_COMMAND_LIST_TYPE type);
		~CommandQueue();
	public:
		CommandList* RequestCommandList();
		CommandList* RequestCommandList(u64 commandID);
		void Begin();
		void End();
		void SubmitAndFlush();
		void Flush();

		ID3D12CommandQueue* Get() const {
			return mD3DCmdQueue.Get();
		}

		
	private:
		SharedPtr<CommandList> CreateCommandList();
	};
}


