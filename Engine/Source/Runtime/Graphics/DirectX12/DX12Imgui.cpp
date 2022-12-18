#include "PCH/PCH.h"
#include "DX12Imgui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

PDX12Imgui::PDX12Imgui()
{
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.Fonts->AddFontFromFileTTF("../../Source/Font/Consolas.ttf", 16.0f);
	//ImGui::StyleColorsDark();

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	//ImGuiStyle& style = ImGui::GetStyle();
	//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	//{
	//	style.WindowRounding = 0.0f;
	//	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	//}

	////WindowsWindow::AddWindowProcCallBack(ImGui_ImplWin32_WndProcHandler);
	//ImGui_ImplWin32_Init((void*)handle);
	//DirectX12::gSrvDescriptorHeap = CreateD3DDescriptorHeap(DirectX12API::GetD3DDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
	//	D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, DirectX12::gMaxSrvCount * bufferCount);
	//DirectX12::gCommandAlloc = CreateD3DCommandAllocator(DirectX12API::GetD3DDevice(), D3D12_COMMAND_LIST_TYPE_DIRECT);
	//DirectX12::gCommandList = CreateD3DCommandList(DirectX12API::GetD3DDevice(), DirectX12::gCommandAlloc, D3D12_COMMAND_LIST_TYPE_DIRECT);
	//DirectX12::gCommandList->Close();
	//ImGui_ImplDX12_Init(DirectX12API::GetD3DDevice(), (i32)DirectX12API::GetInstance()->GetBufferCount(),
	//	DXGI_FORMAT_R8G8B8A8_UNORM, DirectX12::gSrvDescriptorHeap.Get(),
	//	DirectX12::gSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
	//	DirectX12::gSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	//DirectX12::gCurrentSrvIndex = (DirectX12::gMaxSrvCount * DirectX12API::GetInstance()->GetBufferIndex()) + DirectX12::gSrvStartIndex;
	//DirectX12::gIncreaseSize = DirectX12API::GetD3DDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	//auto dx12FrameBuffer = static_cast<DirectX12FrameBuffer*>(mFrameBuffer.get());
	//dx12FrameBuffer->AddUpdateCallBack(
	//	[&](ID3D12GraphicsCommandList* cmdList, ID3D12Resource* backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_RESOURCE_STATES state) ->D3D12_RESOURCE_STATES
	//{
	//	if (state != D3D12_RESOURCE_STATE_RENDER_TARGET)
	//	{
	//		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
	//			backBuffer, state, D3D12_RESOURCE_STATE_RENDER_TARGET));
	//		state = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//	}
	//	cmdList->OMSetRenderTargets(1, &rtv, FALSE, NULL);
	//	cmdList->SetDescriptorHeaps(1, DirectX12::gSrvDescriptorHeap.GetAddressOf());
	//	ImGui::Render();
	//	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);


	//	DirectX12::gCurrentSrvIndex = (DirectX12::gMaxSrvCount * DirectX12API::GetInstance()->GetBufferIndex()) + DirectX12::gSrvStartIndex;
	//	return state;
	//});

	//dx12FrameBuffer->AddPresentCallBack([&]()
	//{
	//	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	//	{
	//		ImGui::UpdatePlatformWindows();
	//		ImGui::RenderPlatformWindowsDefault(NULL, (void*)DirectX12::gCommandList.Get());
	//	}
	//});
}

PDX12Imgui::~PDX12Imgui()
{
	//auto window = Application::GetInstance().GetWindow();
	//auto platform = window->GetPlatform();
	//auto api = JGGraphics::GetInstance().GetDesc().GraphicsAPI;
	//DirectX12::gSrvDescriptorHeap.Reset(); DirectX12::gSrvDescriptorHeap = nullptr;
	//DirectX12::gCommandList.Reset(); DirectX12::gCommandList = nullptr;
	//DirectX12::gCommandAlloc.Reset(); DirectX12::gCommandAlloc = nullptr;
	//ImGui_ImplDX12_Shutdown();

	//ImGui_ImplWin32_Shutdown();
	//ImGui::DestroyContext();
}

ImTextureID PDX12Imgui::ConvertImGuiTextureID(TextureID id)
{
	return ImTextureID();
	//CD3DX12_CPU_DESCRIPTOR_HANDLE cpu(DirectX12::gSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	//CD3DX12_GPU_DESCRIPTOR_HANDLE gpu(DirectX12::gSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	//cpu.Offset((i32)DirectX12::gCurrentSrvIndex, (u32)DirectX12::gIncreaseSize);
	//gpu.Offset((i32)DirectX12::gCurrentSrvIndex, (u32)DirectX12::gIncreaseSize);
	//DirectX12::gCurrentSrvIndex++;
	//DirectX12API::GetD3DDevice()->CopyDescriptorsSimple(1, cpu, { id }, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//return gpu.ptr;
	//return ImTextureID();
}

void PDX12Imgui::NewFrame()
{
	//auto api = JGGraphics::GetInstance().GetDesc().GraphicsAPI;

	//switch (api)
	//{
	//case JG::EGraphicsAPI::DirectX12:
	//	//DirectX12::gCurrentSrvIndex = DirectX12::gSrvStartIndex;
	//	ImGui_ImplDX12_NewFrame();
	//	break;
	//}

	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();
}
