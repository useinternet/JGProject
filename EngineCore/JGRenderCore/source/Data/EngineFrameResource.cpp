#include"EngineFrameResource.h"
#include"ResourceManagement/DataManager.h"
#include"Data/CommonData.h"
using namespace std;
using namespace JGRC;
FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT lightCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));
	PassCB = std::make_unique<UploadBuffer<PassConstantData>>(device, passCount, true);
	ObjectCB = std::make_unique<UploadBuffer<ObjectConstantData>>(device, objectCount, true);
	SkinnedCB = std::make_unique<UploadBuffer<SkinnedConstantData>>(device, std::max<UINT>(1,
		CommonData::_DataManager()->SkinnedDataCount()), true);
	MaterialCB = std::make_unique<UploadBuffer<MaterialConstantData>>(device, materialCount, false);
	LightCB = make_unique<UploadBuffer<Light>>(device, lightCount, false);

}
void FrameResource::ReSize(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT lightCount)
{
	PassCB.reset();
	ObjectCB.reset();
	MaterialCB.reset();
	LightCB.reset();
	PassCB = std::make_unique<UploadBuffer<PassConstantData>>(device, passCount, true);
	ObjectCB = std::make_unique<UploadBuffer<ObjectConstantData>>(device, objectCount, true);
	SkinnedCB = std::make_unique<UploadBuffer<SkinnedConstantData>>(device, std::max<UINT>(1, CommonData::_DataManager()->SkinnedDataCount()), true);
	MaterialCB = std::make_unique<UploadBuffer<MaterialConstantData>>(device, materialCount, false);
	LightCB = make_unique<UploadBuffer<Light>>(device, lightCount, false);
}
D3D12_GPU_VIRTUAL_ADDRESS FrameResource::ObjectCBHeapAddress(ObjectData* Data)
{
	D3D12_GPU_VIRTUAL_ADDRESS result = ObjectCB->Resource()->GetGPUVirtualAddress();
	result += (Data->Index() * Data->Size());
	return result;
}
D3D12_GPU_VIRTUAL_ADDRESS FrameResource::PassCBHeapAddress(PassData* Data)
{
	D3D12_GPU_VIRTUAL_ADDRESS result = PassCB->Resource()->GetGPUVirtualAddress();
	result += (Data->Index() * Data->Size());
	return result;
}
D3D12_GPU_VIRTUAL_ADDRESS FrameResource::SkinnedCBHeapAddress(SkinnedData* Data)
{
	D3D12_GPU_VIRTUAL_ADDRESS result = SkinnedCB->Resource()->GetGPUVirtualAddress();
	result += (Data->Index() * Data->Size());
	return result;
}
void EngineFrameResourceManager::BuildFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT lightCount)
{
	m_Device = device;
	for (UINT i = 0; i < CPU_FRAMERESOURCE_NUM; ++i)
	{
		m_FrameResources.push_back(make_unique<FrameResource>(device,
			passCount, objectCount, materialCount, lightCount));
	}
}
void EngineFrameResourceManager::Resize(UINT passCount, UINT objectCount, UINT materialCount, UINT lightCount)
{
	for (UINT i = 0; i < CPU_FRAMERESOURCE_NUM; ++i)
	{
		m_FrameResources[i]->ReSize(m_Device, passCount, objectCount, materialCount, lightCount);
	}
}
void EngineFrameResourceManager::FrameResourcePerFrame(ID3D12Fence* Fence)
{
	m_CurrentFrameResourceIndex = (m_CurrentFrameResourceIndex + 1) % CPU_FRAMERESOURCE_NUM;
	m_CurrentFrameResource = m_FrameResources[m_CurrentFrameResourceIndex].get();

	if (m_CurrentFrameResource->Fence != 0 && Fence->GetCompletedValue() < m_CurrentFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(Fence->SetEventOnCompletion(m_CurrentFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

}