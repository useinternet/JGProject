#include<PCH.h>
#include"CommandQueue.h"
#include"CommandList.h"
#include"DxUtil.h"
#include"Fence.h"
using namespace Dx12;
using namespace std;



CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type)
{
	m_CmdType = type;
	m_D3D_CmdQeueue = DxUtil::CreateCommandQueue(type);

	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		m_D3D_CmdQeueue->SetName(L"Direct Command Queue");
		break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		m_D3D_CmdQeueue->SetName(L"Compute Command Queue");
		break;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		m_D3D_CmdQeueue->SetName(L"Copy Command Queue");
		break;
	}
	m_Fence = make_unique<Fence>();
}
ID3D12CommandQueue* CommandQueue::Get() const
{
	return m_D3D_CmdQeueue.Get();
}
CommandList* CommandQueue::GetCommandList()
{
	CommandList* result = nullptr;
	if (m_PendingCommandList.empty())
	{
		auto cmdList = make_unique<CommandList>(m_CmdType);
		result = cmdList.get();
		m_CommandListPool[cmdList.get()] = move(cmdList);
	}
	else
	{
		result = m_PendingCommandList.front().get();
		result->Reset();
		m_CommandListPool[result] = m_PendingCommandList.front();
		m_PendingCommandList.pop();
	}

	return result;
}
uint64_t CommandQueue::Signal()
{
	m_Fence->IncreaseValue();
	ThrowIfFailed(m_D3D_CmdQeueue->Signal(m_Fence->Get(), m_Fence->GetValue()));
	return m_Fence->GetValue();
}
void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
{
	m_Fence->WaitForFenceValue(fenceValue);
}
void CommandQueue::Flush()
{
	uint64_t value = Signal();
	WaitForFenceValue(value);


}

void CommandQueue::ExcuteCommandList(CommandList* cmdList)
{
	ID3D12CommandList* d3d_cmdLists[] = { cmdList->Get() };
	m_D3D_CmdQeueue->ExecuteCommandLists(1, d3d_cmdLists);

	m_PendingCommandList.push(m_CommandListPool[cmdList]);
	m_CommandListPool.erase(cmdList);
}
void CommandQueue::ExcuteCommandLists(uint32_t numCmdList, CommandList** cmdLists)
{
	std::vector<ID3D12CommandList*> d3d_cmdLists(numCmdList);
	for (uint32_t i = 0; i < numCmdList; ++i)
	{
		d3d_cmdLists[i] = cmdLists[i]->Get();

		m_PendingCommandList.push(m_CommandListPool[cmdLists[i]]);
		m_CommandListPool.erase(cmdLists[i]);
	}
	m_D3D_CmdQeueue->ExecuteCommandLists(numCmdList, d3d_cmdLists.data());
}