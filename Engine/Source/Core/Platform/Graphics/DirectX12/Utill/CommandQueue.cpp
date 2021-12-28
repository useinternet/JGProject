#include "pch.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "Fence.h"
#include "ResourceStateTracker.h"
#include "../DirectX12API.h"


namespace JG
{
	CommandQueue::CommandQueue(u64 bufferCount, D3D12_COMMAND_LIST_TYPE type)
	{
		mD3DType = type;
		mFence = CreateUniquePtr<Fence>();
		mD3DCmdQueue = CreateD3DCommandQueue(DirectX12API::GetD3DDevice(), mD3DType);
		mFenceValue.resize(bufferCount, 0);

		auto bufferCnt = DirectX12API::GetFrameBufferCount();
		for (u64 i = 0; i < bufferCnt; ++i)
		{
			mExcuteCmdLists[i] = SortedDictionary<u64, SharedPtr<CommandList>>();
			mExcutePendingCmdLists[i] = SortedDictionary<u64, SharedPtr<CommandList>>();
		}
	}

	CommandQueue::~CommandQueue() = default;

	CommandList* CommandQueue::RequestCommandList(u64 ID)
	{
		while (mIsCommandListExcute == true) {}
	

		std::lock_guard<std::mutex> lock(mMutex);
		auto& CmdLists = mExcuteCmdLists[DirectX12API::GetFrameBufferIndex()];
		auto& PendingCmdLists = mExcutePendingCmdLists[DirectX12API::GetFrameBufferIndex()];

		if (CmdLists.find(ID) == CmdLists.end())
		{
			auto pCmdList = CreateCommandList();
			CmdLists[ID] = pCmdList;
			PendingCmdLists[ID] = CreateCommandList();
			return CmdLists[ID].get();
		}
		else
		{
			auto pCmdList = CmdLists[ID].get();
			auto pPendingCmdList = PendingCmdLists[ID].get();
			if (pCmdList->IsClosing())
			{
				pCmdList->Reset();
				pPendingCmdList->Reset();
			}
			return pCmdList;
		}
	}

	void CommandQueue::Begin()
	{
		uint64_t value = mFenceValue[DirectX12API::GetFrameBufferIndex()];
		mFence->WaitForFenceValue(value);
	}

	void CommandQueue::End()
	{
		mIsCommandListExcute = true;
		List<ID3D12CommandList*>   d3dCmdLists;
		ResourceStateTracker::Lock();

		auto& PendingCmdLists = mExcutePendingCmdLists[DirectX12API::GetFrameBufferIndex()];
		for (auto cmdList : mExcuteCmdLists[DirectX12API::GetFrameBufferIndex()])
		{
			if (cmdList.second->IsClosing() == true) continue;
			auto pendCmdList = PendingCmdLists[cmdList.first];
			bool has_pending_barrier = cmdList.second->Close(pendCmdList.get());

			pendCmdList->Close();
			if (has_pending_barrier)
			{
				d3dCmdLists.push_back(pendCmdList->Get());
			}
			d3dCmdLists.push_back(cmdList.second->Get());
		}
		ResourceStateTracker::UnLock();
		mD3DCmdQueue->ExecuteCommandLists((uint32_t)d3dCmdLists.size(), d3dCmdLists.data());

		mFence->IncreaseValue();
		mD3DCmdQueue->Signal(mFence->Get(), mFence->GetValue());
		mFenceValue[DirectX12API::GetFrameBufferIndex()] = mFence->GetValue();
		mIsCommandListExcute = false;
	}

	void CommandQueue::Flush()
	{
		mFence->IncreaseValue();
		mD3DCmdQueue->Signal(mFence->Get(), mFence->GetValue());
		mFence->WaitForFenceValue(mFence->GetValue());
	}
	SharedPtr<CommandList> CommandQueue::CreateCommandList()
	{
		SharedPtr<CommandList> cmdList = nullptr;
		switch (mD3DType)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			cmdList = CreateSharedPtr<GraphicsCommandList>(mD3DType);
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			cmdList = CreateSharedPtr<ComputeCommandList>(mD3DType);
			break;
		default:
			cmdList = CreateSharedPtr<CommandList>(mD3DType);
			break;
		}

		return cmdList;
	}
}
