#include "PCH/PCH.h"
#include "DX12JGGui.h"
#include "DirectX12/DirectX12API.h"
#include "DirectX12/DX12FrameBuffer.h"
#include "GUI.h"
#include "External/Classes/ImGuiBuild.h"
#include "External/Classes/ImGuiMenuBuild.h"
#include "External/imgui/imgui.h"
#include "External/imgui/imgui_impl_win32.h"
#include "External/imgui/imgui_impl_dx12.h"

#ifdef _PLATFORM_WINDOWS
#include "Platform/Windows/WindowsJWindow.h"
#endif

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

PDX12JGGui::PDX12JGGui()
{
	
}

PDX12JGGui::~PDX12JGGui()
{

}

void PDX12JGGui::Create()
{
	GGUIGlobalSystem::GetInstance().SetGUIBuild(Allocate<PImGuiBuild>());
	GGUIGlobalSystem::GetInstance().SetMenuBuild(Allocate<PImGuiMenuBuild>());

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

	HCoreSystemGlobalValues& globalValues = GCoreSystem::GetGlobalValues();
#ifdef _PLATFORM_WINDOWS
	globalValues.WindowCallBacks->WndProc.AddSP(SharedWrap(this), &PDX12JGGui::OnWndProc);
	globalValues.WindowCallBacks->OnResize.AddSP(SharedWrap(this), &PDX12JGGui::OnResize);

	ImGui_ImplWin32_Init((void*)globalValues.MainWindow->GetHandle());
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
	_increaseSize = HDirectXAPI::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	PSharedPtr<PDX12FrameBuffer> frameBuffer = HDirectXAPI::GetFrameBuffer();

	frameBuffer->OnUpdate.AddSP(SharedWrap(this), &PDX12JGGui::OnUpdate);
	frameBuffer->OnPresent.AddSP(SharedWrap(this), &PDX12JGGui::OnPresent);
}

void PDX12JGGui::Destroy()
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

uint64 PDX12JGGui::GPUAllocate(TextureID textureID)
{
	return (uint64)ConvertImGuiTextureID(textureID);
}

ImTextureID PDX12JGGui::ConvertImGuiTextureID(TextureID id)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE CPU(_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPU(_srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	CPU.Offset((int32)_currentSrvIndex, (uint32)_increaseSize);
	GPU.Offset((int32)_currentSrvIndex, (uint32)_increaseSize);
	_currentSrvIndex++;

	HDirectXAPI::GetDevice()->CopyDescriptorsSimple(1, CPU, { id }, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	return (ImTextureID)GPU.ptr;
}

void PDX12JGGui::NewFrame()
{
	_currentSrvIndex = _srvStartIndex;
	ImGui_ImplDX12_NewFrame();

#ifdef _PLATFORM_WINDOWS
	ImGui_ImplWin32_NewFrame();
#endif
	ImGui::NewFrame();

	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGuiWindowFlags   window_flags    = ImGuiWindowFlags_NoDocking;
	const ImGuiViewport* viewport      = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	window_flags |= ImGuiWindowFlags_NoBackground;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(HGUIConstants::MainWidgetID.GetCStr(), nullptr, window_flags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);
	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("UISystemLayer DockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();
}

void PDX12JGGui::OnUpdate(HDX12CommandList* cmdList, HDX12Resource* backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_RESOURCE_STATES& outState)
{
	if (outState != D3D12_RESOURCE_STATE_RENDER_TARGET)
	{
		CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, outState, D3D12_RESOURCE_STATE_RENDER_TARGET);

		cmdList->ResourceBarrier(1, &resourceBarrier);
		outState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	cmdList->OMSetRenderTargets(1, &rtv, FALSE, NULL);
	cmdList->SetDescriptorHeaps(1, _srvDescriptorHeap.GetAddressOf());
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);

	_currentSrvIndex = _srvStartIndex;
}

void PDX12JGGui::OnPresent()
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault(NULL, (void*)_commandList.Get());
	}
}

void PDX12JGGui::OnResize(uint32 inWidth, uint32 inHeight)
{
	HCoreSystemGlobalValues& globalValues = GCoreSystem::GetGlobalValues();
	HVector2Int clientSize = globalValues.MainWindow->GetClientSize();

	PSharedPtr<PDX12FrameBuffer> frameBuffer = HDirectXAPI::GetFrameBuffer();
	frameBuffer->Resize(clientSize.x, clientSize.y);
}

void PDX12JGGui::OnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}
