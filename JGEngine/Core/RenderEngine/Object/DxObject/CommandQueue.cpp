#include "pch.h"
#include "CommandQueue.h"
#include "DxUtill.h"
#include "CommandList.h"
#include "ResourceDataMap.h"
#include "Fence.h"
#include "RenderDevice.h"
using namespace std;

namespace RE
{
	CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type) :
		DxObject("CommandQueue"),
		m_Type(type)
	{
		m_D3D_CmdQueue = CreateD3DCommandQueue(GetD3DDevice(), type);

		m_Fence = make_shared<Fence>();
	}


	uint64_t CommandQueue::Signal()
	{
		m_Fence->IncreaseValue();
		m_D3D_CmdQueue->Signal(m_Fence->Get(), m_Fence->GetValue());
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
	
	void CommandQueue::ExcuteCommandList(const std::vector<CommandList*> cmdLists)
	{
	
		ResourceDataMap::StateLock();
		std::vector<ID3D12CommandList*> d3d_cmdLists;
		std::vector<CommandList*> temp_pending_commandList;
		uint32_t value_index = GetRenderDevice()->GetValueIndex();
		for (auto& commandList : cmdLists)
		{
			auto pendingCmdList = GetCommandList();
			bool hasPendingBarriers = commandList->Close(pendingCmdList);
			pendingCmdList->Close();
			
			if (hasPendingBarriers)
			{
				//RE_LOG_INFO("{2} : {0} index's PendingCommandList({1}) hasPendingBarriers", value_index, (uint64_t)pendingCmdList, (int)m_Type);
				d3d_cmdLists.push_back(pendingCmdList->GetD3DCommandList());
			}
			d3d_cmdLists.push_back(commandList->GetD3DCommandList());
			temp_pending_commandList.push_back(pendingCmdList);
			temp_pending_commandList.push_back(commandList);
		}
		for (auto& commandList : temp_pending_commandList)
		{
			// RE_LOG_INFO("{2} : {0} index's CommandList({1}) -> PendingCmdList", value_index, (uint64_t)commandList, (int)m_Type);
			m_PendingCmdList[value_index].push(m_CmdListPool[commandList]);
			m_CmdListPool.erase(commandList);
		}

		m_D3D_CmdQueue->ExecuteCommandLists((uint32_t)d3d_cmdLists.size(), d3d_cmdLists.data());

		ResourceDataMap::StateUnLock();
	}
	CommandList* CommandQueue::GetCommandList()
	{
		std::lock_guard<mutex> lock(m_Mutex);

	
		uint32_t value_index = GetRenderDevice()->GetValueIndex();
		CommandList* result = nullptr;
		if (m_PendingCmdList[value_index].empty())
		{
			std::shared_ptr<CommandList> cmdList = make_shared<CommandList>(m_Type);
			RE_LOG_INFO("{2} : {0} index's CmdList({1})  Create", value_index, (uint64_t)cmdList.get(), (int)m_Type);
			result = cmdList.get();
			m_CmdListPool[cmdList.get()] = move(cmdList);
		}
		else 
		{

			result = m_PendingCmdList[value_index].front().get();
			RE_LOG_INFO("{2} : {0} index's PendingCmdList({1}) -> CmdList", value_index, (uint64_t)result, (int)m_Type);
			result->Reset();
			m_CmdListPool[result] = move(m_PendingCmdList[value_index].front());

			m_PendingCmdList[value_index].pop();
		}
		return result;

	}
}