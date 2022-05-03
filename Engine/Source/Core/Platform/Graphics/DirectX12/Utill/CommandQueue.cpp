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

		auto bufferCnt = DirectX12API::GetInstance()->GetBufferCount();
		for (u64 i = 0; i < bufferCnt; ++i)
		{
			mExcuteCmdLists[i]		  = SortedDictionary<std::thread::id, SharedPtr<CommandList>>();
			mExcutePendingCmdLists[i] = SortedDictionary<std::thread::id, SharedPtr<CommandList>>();
		}
	}

	CommandQueue::~CommandQueue() = default;

	CommandList* CommandQueue::RequestCommandList()
	{
		while (mIsCommandListExcute == true) {}
		

		std::lock_guard<std::mutex> lock(mMutex);
		std::thread::id curr_thread_id = std::this_thread::get_id();

		u64 buffIndex = DirectX12API::GetInstance()->GetBufferIndex();

		auto& CmdLists = mExcuteCmdLists[buffIndex];
		auto& PendingCmdLists = mExcutePendingCmdLists[buffIndex];

		if (CmdLists.find(curr_thread_id) == CmdLists.end())
		{
			auto pCmdList = CreateCommandList();
			CmdLists[curr_thread_id] = pCmdList;
			PendingCmdLists[curr_thread_id] = CreateCommandList();
			return CmdLists[curr_thread_id].get();
		}
		else
		{
			auto pCmdList = CmdLists[curr_thread_id].get();
			auto pPendingCmdList = PendingCmdLists[curr_thread_id].get();
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
		u64 buffIndex = DirectX12API::GetInstance()->GetBufferIndex();
		uint64_t value = mFenceValue[buffIndex];
		mFence->WaitForFenceValue(value);
	}

	void CommandQueue::End()
	{
		mIsCommandListExcute = true;
		List<ID3D12CommandList*>   d3dCmdLists;
		ResourceStateTracker::Lock();
		u64 buffIndex = DirectX12API::GetInstance()->GetBufferIndex();
		auto& PendingCmdLists = mExcutePendingCmdLists[buffIndex];
		for (auto cmdList : mExcuteCmdLists[buffIndex])
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
		mFenceValue[buffIndex] = mFence->GetValue();
		mIsCommandListExcute = false;
	}

	void CommandQueue::SubmitAndFlush()
	{
		End();
		Flush();
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
