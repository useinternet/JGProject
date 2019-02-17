#pragma once
#include"DxCommon/DxCommon.h"
#include"HeapArray.h"
#include"Data/Hash.h"

namespace JGRC
{
	class DxDevice;
	class CommandListManager;
	class DxHeapManager;
	class RCORE_EXPORT DxResource
	{
		friend DxHeapManager;
	private:
		UINT m_ID = 0;
		bool      m_IsRegisterHeap    = false;
		EHeapType m_RegisteredHeapType = EHeapType::NONE;
	public:
		Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
		D3D12_RESOURCE_DESC   Desc;
		D3D12_HEAP_PROPERTIES Property;
		D3D12_HEAP_FLAGS      Flag;
		D3D12_RESOURCE_STATES State;
		D3D12_CLEAR_VALUE     ClearValue;
	public:
		UINT ID() const {
			return m_ID;
		}
		bool IsRegisterHeap() const {
			return m_IsRegisterHeap;
		}
	};
	class RCORE_EXPORT DxHeapManager
	{
		typedef std::pair<DxResource*, DxResource*> UavID;
		typedef std::vector<std::shared_ptr<CbvData>> CbvDataMemorys;
		typedef std::unordered_map<DxResource*, std::shared_ptr<SrvData>> SrvDataMemorys;
		typedef std::unordered_map<DxResource*, std::shared_ptr<RtvData>> RtvDataMemorys;
		typedef std::unordered_map<DxResource*, std::shared_ptr<DsvData>> DsvDataMemorys;
		typedef std::unordered_map<UavID, std::shared_ptr<UavData>, JGLibrary::pair_hash> UavDataMemorys;
		DxDevice* m_Device = nullptr; 
	private:
		HeapArray<EHeapType::CBV, CbvData> m_CbvHeap;
		HeapArray<EHeapType::SRV, SrvData> m_SrvHeap;
		HeapArray<EHeapType::UAV, UavData> m_UavHeap;
		HeapArray<EHeapType::RTV, RtvData> m_RtvHeap;
		HeapArray<EHeapType::DSV, DsvData> m_DsvHeap;
	private:
		std::vector<std::shared_ptr<DxResource>> m_ResourceMems;
		CbvDataMemorys m_CbvDataMems;
		SrvDataMemorys m_SrvDataMems;
		UavDataMemorys m_UavDataMems;
		RtvDataMemorys m_RtvDataMems;
		DsvDataMemorys m_DsvDataMems;
	public:
		void CreateManager(DxDevice* Device);
		void RegisterHeap(CommandListManager* CmdListManager, UINT idx);
		void ResourceStateTransition(
			CommandListManager* CmdListManager,
			UINT CmdIdx,
			DxResource* resource,
			D3D12_RESOURCE_STATES State);
	public:
		DxResource* BuildResource(
			D3D12_RESOURCE_DESC* Desc,
			D3D12_CLEAR_VALUE* ClearValue,
			D3D12_RESOURCE_STATES State    = D3D12_RESOURCE_STATE_COMMON,
			D3D12_HEAP_PROPERTIES Property = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAGS Flag          = D3D12_HEAP_FLAG_NONE);
		void ReBuildResource(DxResource* Resource, DxResource* CounterResource = nullptr);
	public:
		CbvData* CreateCbvData(const D3D12_CONSTANT_BUFFER_VIEW_DESC& Desc);
		SrvData* CreateSrvData(
			const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc,
			DxResource* Resource);
		UavData* CreateUavData(
			const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc, 
			DxResource* Resource, 
			DxResource* CounterResource = nullptr);
		RtvData* CreateRtvData(
			const D3D12_RENDER_TARGET_VIEW_DESC& Desc,
			DxResource* Resource);
		DsvData* CreateDsvData(
			const D3D12_DEPTH_STENCIL_VIEW_DESC& Desc,
			DxResource* Resource);
	public:

	};
}