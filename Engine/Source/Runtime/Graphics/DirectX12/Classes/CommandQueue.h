#pragma once
#include "Core.h"
#include "DirectX12Helper.h"


class PCommandList;
class PFence;
class PCommandQueue : public IMemoryObject
{
	enum ECommandListState
	{
		Open,
		Close,
	};
private:
	HDX12ComPtr<HDX12CommandQueue> _dx12CommandQueue;
	D3D12_COMMAND_LIST_TYPE    _dx12CommandListType;
	PSharedPtr<PFence> _fence;
	uint64 _fenceValue;
	HMutex _mutex;
	bool _bLock = false;

	HAtomicBool _bCommandListExcute = false;
	HMap<uint64, HHashMap<uint64, PSharedPtr<PCommandList>>> _excuteCmdLists;
	HMap<uint64, HHashMap<uint64, PSharedPtr<PCommandList>>> _excutePendingCmdLists;
	HHashMap<PCommandList*, ECommandListState> _cmdListStates;
public:
	PCommandQueue(D3D12_COMMAND_LIST_TYPE type);
	~PCommandQueue();
public:
	PSharedPtr<PCommandList> RequestCommandList(ECommandListType commandListType, uint64 priority = 0);

	void Begin();
	void End();
	void SubmitAndFlush();
	void Flush();

	HDX12CommandQueue* Get() const {
		return _dx12CommandQueue.Get();
	}

private:
	PSharedPtr<PCommandList> RequestCommandList(ECommandListType commandListType, uint64 commandID, uint64 priority);
	PSharedPtr<PCommandList> CreateCommandList(ECommandListType commandListType);
	ECommandListState GetCommandListState(PSharedPtr<PCommandList> cmdList);
	void SetCommandListState(PSharedPtr<PCommandList> cmdList, ECommandListState state);
};