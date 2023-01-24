#include "PCH/PCH.h"
#include "CommandQueue.h"
#include "DirectX12/DirectX12API.h"
#include "DirectX12/Classes/Fence.h"
#include "DirectX12/Classes/CommandList.h"
#include "DirectX12/Classes/ResourceStateTracker.h"

PCommandQueue::PCommandQueue(D3D12_COMMAND_LIST_TYPE type)
{
	_dx12CommandListType = type;
	_fence = Allocate<PFence>();
	_dx12CommandQueue = HDirectX12Helper::CreateD3DCommandQueue(HDirectXAPI::GetDevice(), _dx12CommandListType);
	_fenceValue = 0;
}

PCommandQueue::~PCommandQueue() = default;

PSharedPtr<PCommandList> PCommandQueue::RequestCommandList(ECommandListType commandListType, uint64 priority)
{
	HLockGuard<HMutex> lock(_mutex);

	_bLock = true;

	std::thread::id curr_thread_id = std::this_thread::get_id();
	uint64 hash = std::hash<std::thread::id>()(curr_thread_id);
	
	return RequestCommandList(commandListType, hash, priority);
}

void PCommandQueue::Begin()
{
	_fence->WaitForFenceValue(_fenceValue);
}

void PCommandQueue::End()
{
	_bCommandListExcute = true;

	HList<ID3D12CommandList*>   d3dCmdLists;

	PResourceStateTracker::Lock();

	for (HPair < const uint64, HHashMap<uint64, PSharedPtr<PCommandList>>>& pair : _excuteCmdLists)
	{
		for (HPair<const uint64, PSharedPtr<PCommandList>>& cmdList : pair.second)
		{
			PSharedPtr<PCommandList> pendCmdList = _excutePendingCmdLists[pair.first][cmdList.first];

			if (GetCommandListState(cmdList.second) == ECommandListState::Close) continue;
			if (GetCommandListState(pendCmdList) == ECommandListState::Close) continue;

			bool bHasPendingBarrier = cmdList.second->Close(pendCmdList.GetRawPointer());

			pendCmdList->Close();
			if (bHasPendingBarrier)
			{
				d3dCmdLists.push_back(pendCmdList->Get());
			}
			d3dCmdLists.push_back(cmdList.second->Get());

			SetCommandListState(cmdList.second, ECommandListState::Close);
			SetCommandListState(pendCmdList, ECommandListState::Close);
		}
	}
	
	PResourceStateTracker::UnLock();
	_dx12CommandQueue->ExecuteCommandLists((uint32_t)d3dCmdLists.size(), d3dCmdLists.data());

	_fence->IncreaseValue();
	_dx12CommandQueue->Signal(_fence->Get(), _fence->GetValue());
	_fenceValue = _fence->GetValue();

	_bCommandListExcute = false;
}

void PCommandQueue::SubmitAndFlush()
{
	End();
	Flush();
	Begin();
}

void PCommandQueue::Flush()
{
	_fence->IncreaseValue();
	_dx12CommandQueue->Signal(_fence->Get(), _fence->GetValue());
	_fence->WaitForFenceValue(_fence->GetValue());
}

PSharedPtr<PCommandList> PCommandQueue::RequestCommandList(ECommandListType commandListType, uint64 commandID, uint64 priority)
{
	while (_bCommandListExcute == true) {}

	if (_bLock == false)
	{
		_mutex.lock();
	}

	commandID = 16777619U * (uint64)commandListType ^ commandID;

	PSharedPtr<PCommandList> result = nullptr;
	if (_excuteCmdLists[priority].find(commandID) == _excuteCmdLists[priority].end())
	{
		_excuteCmdLists[priority][commandID] = CreateCommandList(commandListType);
		_excutePendingCmdLists[priority][commandID] = CreateCommandList(commandListType);

		result = _excuteCmdLists[priority][commandID];
	}
	else
	{
		PSharedPtr<PCommandList> pCmdList = _excuteCmdLists[priority][commandID];
		PSharedPtr<PCommandList> pPendingCmdList = _excutePendingCmdLists[priority][commandID];

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
		result = pCmdList;
	}

	if (_bLock == false)
	{
		_mutex.unlock();
	}

	_bLock = false;

	return result;
}

PSharedPtr<PCommandList> PCommandQueue::CreateCommandList(ECommandListType commandListType)
{
	PSharedPtr<PCommandList> cmdList = nullptr;

	switch (_dx12CommandListType)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		switch (commandListType)
		{
		case ECommandListType::Base:
			cmdList = Allocate<PCommandList>(_dx12CommandListType);
			break;
		case ECommandListType::Graphics:
			cmdList = Allocate<PGraphicsCommandList>(_dx12CommandListType);
			break;
		case ECommandListType::Compute:
			cmdList = Allocate<PComputeCommandList>(_dx12CommandListType);
			break;
		}
		break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		switch (commandListType)
		{
		case ECommandListType::Base:
			cmdList = Allocate<PCommandList>(_dx12CommandListType);
			break;

		case ECommandListType::Compute:
			cmdList = Allocate<PComputeCommandList>(_dx12CommandListType);
			break;
		}
		break;
	default:
		break;
	}

	SetCommandListState(cmdList, ECommandListState::Open);

	return cmdList;
}
PCommandQueue::ECommandListState PCommandQueue::GetCommandListState(PSharedPtr<PCommandList> cmdList)
{
	return _cmdListStates[cmdList.GetRawPointer()];
}

void PCommandQueue::SetCommandListState(PSharedPtr<PCommandList> cmdList, ECommandListState state)
{
	_cmdListStates[cmdList.GetRawPointer()] = state;
}