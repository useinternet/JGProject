#pragma once
#include "Classes/JGImgui.h"
#include "DirectX12/Classes/DirectX12Helper.h"
#include "imgui/imgui.h"

class PDX12Imgui
	: public IMemoryObject
	, public IJGImgui
{
	HDX12ComPtr<HDX12DescriptorHeap>   _srvDescriptorHeap;
	HDX12ComPtr<HDX12CommandAllocator> _commandAlloc;
	HDX12ComPtr<HDX12CommandList>      _commandList;

	uint32 _increaseSize    = 0;
	uint32 _currentSrvIndex = 0;
	uint32 _srvStartIndex = 1;
	const uint32 _maxSrvCount = 1024;
public:
	PDX12Imgui();
	virtual ~PDX12Imgui();

public:
	ImTextureID ConvertImGuiTextureID(TextureID id);
	void	NewFrame();

private:
	void OnUpdate(HDX12CommandList* cmdList, HDX12Resource* backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_RESOURCE_STATES& outState);
	void OnPresent();
};