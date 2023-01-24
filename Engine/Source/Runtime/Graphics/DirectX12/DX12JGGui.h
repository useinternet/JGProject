#pragma once
#include "Classes/JGGui.h"
#include "DirectX12/Classes/DirectX12Helper.h"
#include "External/imgui/imgui.h"

class PDX12JGGui
	: public IMemoryObject
	, public IJGGui
{
	HDX12ComPtr<HDX12DescriptorHeap>   _srvDescriptorHeap;
	HDX12ComPtr<HDX12CommandAllocator> _commandAlloc;
	HDX12ComPtr<HDX12CommandList>      _commandList;

	uint32 _increaseSize    = 0;
	uint32 _currentSrvIndex = 0;
	uint32 _srvStartIndex = 1;
	const uint32 _maxSrvCount = 1024;
public:
	PDX12JGGui();
	virtual ~PDX12JGGui();
	void Create();
	void Destroy();
	virtual uint64 GPUAllocate(TextureID textureID) override;
public:
	ImTextureID ConvertImGuiTextureID(TextureID id);
	void	NewFrame();

private:
	void OnUpdate(HDX12CommandList* cmdList, HDX12Resource* backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_RESOURCE_STATES& outState);
	void OnPresent();
	//LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void OnResize(uint32 inWidth, uint32 inHeight);
	void OnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};