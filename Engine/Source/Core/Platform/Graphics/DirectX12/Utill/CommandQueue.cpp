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
			mExcuteCmdLists[i] = SortedDictionary<u64, SharedPtr<CommandList>>();
			mExcutePendingCmdLists[i] = SortedDictionary<u64, SharedPtr<CommandList>>();
		}
	}

	CommandQueue::~CommandQueue() = default;

	CommandList* CommandQueue::RequestCommandList()
	{
		std::lock_guard<std::mutex> lock(mMutex);
		mIsLock = true;
		std::thread::id curr_thread_id = std::this_thread::get_id();
		u64 hash = std::hash<std::thread::id>()(curr_thread_id);
		return RequestCommandList(hash);
	}

	CommandList* CommandQueue::RequestCommandList(u64 commandID)
	{
		while (mIsCommandListExcute == true) {}		
		u64 buffIndex = DirectX12API::GetInstance()->GetBufferIndex();
		auto& CmdLists = mExcuteCmdLists[buffIndex];
		auto& PendingCmdLists = mExcutePendingCmdLists[buffIndex];

		if (mIsLock == false)
		{
			mMutex.lock();
		}

		CommandList* result = nullptr;
		if (CmdLists.find(commandID) == CmdLists.end())
		{
			CmdLists[commandID]        = CreateCommandList();;
			PendingCmdLists[commandID] = CreateCommandList();
			result =  CmdLists[commandID].get();
		}
		else
		{
			auto pCmdList = CmdLists[commandID].get();
			auto pPendingCmdList = PendingCmdLists[commandID].get();

			if (GetCommandListState(pCmdList) == ECommandListState::Close)
			{
				pCmdList->Reset();
				SetCommandListState(pCmdList, ECommandListState::Open);
			}
			if (GetCommandListState(pPendingCmdList) == ECommandListState::Close)
			{
				pPendingCmdList->Reset();
				SetCommandListState(pPendingCmdList, ECommandListState::Open);
			}
			result =  pCmdList;
		}

		if (mIsLock == false)
		{
			mMutex.unlock();
		}
		mIsLock = false;

		return result;
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
			auto pendCmdList = PendingCmdLists[cmdList.first];
			if (GetCommandListState(cmdList.second.get()) == ECommandListState::Close) continue;
			if (GetCommandListState(pendCmdList.get()) == ECommandListState::Close) continue;

			
			bool has_pending_barrier = cmdList.second->Close(pendCmdList.get());

			pendCmdList->Close();
			if (has_pending_barrier)
			{
				d3dCmdLists.push_back(pendCmdList->Get());
			}
			d3dCmdLists.push_back(cmdList.second->Get());

			SetCommandListState(cmdList.second.get(), ECommandListState::Close);
			SetCommandListState(pendCmdList.get(), ECommandListState::Close);
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
		Begin();
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
		SetCommandListState(cmdList.get(), ECommandListState::Open);
		return cmdList;
	}
	CommandQueue::ECommandListState CommandQueue::GetCommandListState(CommandList* cmdList)
	{
		return mCmdListStates[cmdList];
	}
	void CommandQueue::SetCommandListState(CommandList* cmdList, ECommandListState state)
	{
		mCmdListStates[cmdList] = state;
	}
}
