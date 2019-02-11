#pragma once
#include"DxCommon/DxCommon.h"



namespace JGRC
{
	class GpuCpuSynchronizer;
	class CommandListManager
	{
	public:
		struct CommandList
		{
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    Allocator;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CmdList;
		};
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
		std::vector<CommandList>                   m_CommandLists;
		ID3D12Device* m_Device = nullptr;
	public:
		void FlushCommandQueue(GpuCpuSynchronizer* GCS);
		void CreateManager(ID3D12Device* device)
		{
			m_Device = device;
			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

			ThrowIfFailed(device->CreateCommandQueue(
				&queueDesc,
				IID_PPV_ARGS(m_CommandQueue.GetAddressOf())));
		}
		CommandList* AddCommandList()
		{
			CommandList list;
			ThrowIfFailed(m_Device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(list.Allocator.GetAddressOf())));

			ThrowIfFailed(m_Device->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				list.Allocator.Get(),
				nullptr,
				IID_PPV_ARGS(list.CmdList.GetAddressOf())));

			list.CmdList->Close();
			m_CommandLists.push_back(std::move(list));
			auto result = m_CommandLists.end() - 1;
			return &(*result);
		}
		void Close(UINT idx)
		{
			ThrowIfFailed(m_CommandLists[idx].CmdList->Close());
		}
		void Close()
		{
			for (UINT i = 0; i < (UINT)m_CommandLists.size(); ++i)
			{
				ThrowIfFailed(m_CommandLists[i].CmdList->Close());
			}
		}
		void Reset(UINT idx)
		{
			ThrowIfFailed(m_CommandLists[idx].CmdList->Reset(m_CommandLists[idx].Allocator.Get(), nullptr));
		}
		void Reset()
		{
			for (UINT i = 0; i < (UINT)m_CommandLists.size(); ++i)
			{
				ThrowIfFailed(m_CommandLists[i].CmdList->Reset(m_CommandLists[i].Allocator.Get(), nullptr));
			}
		}
		void CloseAndReset(UINT idx)
		{
			Close(idx);
			Reset(idx);
		}
		void CloseAndReset()
		{
			for (UINT i = 0; i < (UINT)m_CommandLists.size(); ++i)
			{
				Close(i);
				Reset(i);
			}
		}
		void ExcuteCommandLists()
		{
			UINT len = (UINT)m_CommandLists.size();
			std::vector<ID3D12CommandList*> cmdlist(len);
			for (UINT i = 0; i < len; ++i)
			{
				ThrowIfFailed(m_CommandLists[i].CmdList->Close());
				cmdlist[i] = m_CommandLists[i].CmdList.Get();
			}
			m_CommandQueue->ExecuteCommandLists((UINT)cmdlist.size(), cmdlist.data());
		}
		void Signal(ID3D12Fence* fence, UINT64 Offset)
		{
			m_CommandQueue->Signal(fence, Offset);
		}
		ID3D12GraphicsCommandList* GetCommandList(UINT index) const
		{
			if (m_CommandLists.empty())
				return nullptr;
			else
				return m_CommandLists[index].CmdList.Get();
		}
		ID3D12CommandAllocator* GetCommandAlloc(UINT index) const
		{
			if (m_CommandLists.empty())
				return nullptr;
			else
				return m_CommandLists[index].Allocator.Get();
		}
		ID3D12CommandQueue* CommandQue() const
		{
			return m_CommandQueue.Get();
		}

	};
}