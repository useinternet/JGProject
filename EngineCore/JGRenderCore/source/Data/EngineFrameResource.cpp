#include"EngineFrameResource.h"
using namespace std;
using namespace JGRC;
FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT lightCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));
	PassCB = std::make_unique<UploadBuffer<cbPassConstant>>(device, passCount, true);
	ObjectCB = std::make_unique<UploadBuffer<cbObjectConstant>>(device, objectCount, true);
	MaterialCB = std::make_unique<UploadBuffer<MaterialData>>(device, materialCount, false);
	LightCB = make_unique<UploadBuffer<Light>>(device, lightCount, false);
}
void FrameResource::ReSize(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT lightCount)
{
	PassCB.reset();
	ObjectCB.reset();
	MaterialCB.reset();
	LightCB.reset();
	PassCB = std::make_unique<UploadBuffer<cbPassConstant>>(device, passCount, true);
	ObjectCB = std::make_unique<UploadBuffer<cbObjectConstant>>(device, objectCount, true);
	MaterialCB = std::make_unique<UploadBuffer<MaterialData>>(device, materialCount, false);
	LightCB = make_unique<UploadBuffer<Light>>(device, lightCount, false);
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