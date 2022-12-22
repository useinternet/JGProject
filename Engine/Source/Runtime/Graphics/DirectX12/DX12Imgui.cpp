#include "PCH/PCH.h"
#include "DX12Imgui.h"
#include "DirectX12/DirectX12API.h"
#include "DirectX12/DX12FrameBuffer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"

#ifdef _PLATFORM_WINDOWS
#include "Platform/Windows/WindowsJWindow.h"
#endif

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

PDX12Imgui::PDX12Imgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.Fonts->AddFontFromFileTTF("../../Source/Font/Consolas.ttf", 16.0f);
	ImGui::StyleColorsDark();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

#ifdef _PLATFORM_WINDOWS
	PWindowsJWindow::AddWindowProcCallBack(ImGui_ImplWin32_WndProcHandler);
	ImGui_ImplWin32_Init((void*)GCoreSystem::GetGlobalValues().WindowHandle);
#endif


	_srvDescriptorHeap = HDirectX12Helper::CreateD3DDescriptorHeap(HDirectXAPI::GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, _maxSrvCount);
	_commandAlloc = HDirectX12Helper::CreateD3DCommandAllocator(HDirectXAPI::GetDevice(), D3D12_COMMAND_LIST_TYPE_DIRECT);
	_commandList = HDirectX12Helper::CreateD3DCommandList(HDirectXAPI::GetDevice(), _commandAlloc, D3D12_COMMAND_LIST_TYPE_DIRECT);
	_commandList->Close();

	ImGui_ImplDX12_Init(
		HDirectXAPI::GetDevice(), HDirectXAPI::GetArguments().BufferCount, DXGI_FORMAT_R16G16B16A16_FLOAT, 
		_srvDescriptorHeap.Get(),
		_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		_srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	_currentSrvIndex = _srvStartIndex;
	_increaseSize    = HDirectXAPI::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	PSharedPtr<PDX12FrameBuffer> frameBuffer = HDirectXAPI::GetFrameBuffer();

	frameBuffer->OnUpdate.Add<PDX12Imgui>(SharedWrap(this), JG_DELEGATE_FN_BIND_FOURPARAM(PDX12Imgui::OnUpdate));
	frameBuffer->OnPresent.Add<PDX12Imgui>(SharedWrap(this), JG_DELEGATE_FN_BIND(PDX12Imgui::OnPresent));
}

PDX12Imgui::~PDX12Imgui()
{
	_srvDescriptorHeap.Reset(); _srvDescriptorHeap = nullptr;
	_commandList.Reset(); _commandList = nullptr;
	_commandAlloc.Reset(); _commandAlloc = nullptr;
	ImGui_ImplDX12_Shutdown();

#ifdef _PLATFORM_WINDOWS
	ImGui_ImplWin32_Shutdown();
#endif
	ImGui::DestroyContext();
}

ImTextureID PDX12Imgui::ConvertImGuiTextureID(TextureID id)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE CPU(_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPU(_srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	CPU.Offset((int32)_currentSrvIndex, (uint32)_increaseSize);
	GPU.Offset((int32)_currentSrvIndex, (uint32)_increaseSize);
	_currentSrvIndex++;

	HDirectXAPI::GetDevice()->CopyDescriptorsSimple(1, CPU, { id }, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	return (ImTextureID)GPU.ptr;
}

void PDX12Imgui::NewFrame()
{
	_currentSrvIndex = _srvStartIndex;
	ImGui_ImplDX12_NewFrame();

#ifdef _PLATFORM_WINDOWS
	ImGui_ImplWin32_NewFrame();
#endif
	ImGui::NewFrame();
}

void PDX12Imgui::OnUpdate(HDX12CommandList* cmdList, HDX12Resource* backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_RESOURCE_STATES& outState)
{
	if (outState != D3D12_RESOURCE_STATE_RENDER_TARGET)
	{
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer, outState, D3D12_RESOURCE_STATE_RENDER_TARGET));
		outState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	cmdList->OMSetRenderTargets(1, &rtv, FALSE, NULL);
	cmdList->SetDescriptorHeaps(1, _srvDescriptorHeap.GetAddressOf());
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);

	_currentSrvIndex = _srvStartIndex;
}

void PDX12Imgui::OnPresent()
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault(NULL, (void*)_commandList.Get());
	}
}
