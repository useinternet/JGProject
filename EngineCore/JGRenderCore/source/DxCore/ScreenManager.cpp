#include"ScreenManager.h"
#include"CommandListManager.h"
#include"GpuCpuSynchronizer.h"
using namespace std;
using namespace JGRC;
static ScreenManager::ScreenData dummy;

void ScreenManager::CreateManager(IDXGIFactory4* factory, ResourceManager* manager, GpuCpuSynchronizer* GCS,
	CommandListManager* CmdListManager)
{
	m_Factory = factory;
	m_ResourceManager = manager;
	m_GCS = GCS;
	m_CmdListManager = CmdListManager;
}
void ScreenManager::AddSwapChain(
	const string& name,
	HWND hWnd,
	const DxSetting& set,
	CommandListManager* CmdListManager,
	UINT CmdIndex)
{
	if (m_ScreenDatas.find(name) != m_ScreenDatas.end())
		return;

	ScreenData Screen;
	auto CommandList = CmdListManager->GetCommandList(CmdIndex);
	auto CommandAlloc = CmdListManager->GetCommandAlloc(CmdIndex);

	CmdListManager->Reset(CmdIndex);

	ThrowIfFailed(m_Factory->CreateSwapChain(
		CmdListManager->CommandQue(),
		&GetSwapChainDesc(set),
		Screen.SwapChain.GetAddressOf()));

	CmdListManager->FlushCommandQueue(m_GCS);

	ThrowIfFailed(Screen.SwapChain->ResizeBuffers(
		SwapChainBufferCount,
		set.Width, set.Height,
		BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	
	for (UINT i = 0; i < SwapChainBufferCount; ++i)
	{
		Screen.Buffer[i] = m_ResourceManager->BuildResource(Screen.SwapChain.Get(), i);
		Screen.RtvPack[i] = m_ResourceManager->AddRtv(name + "Rtv" + to_string(i), Screen.Buffer[i], nullptr);
	}
	D3D12_RESOURCE_DESC DepthDesc = GetDepthResourceDesc(set);
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DepthBufferFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	Screen.DepthBuffer = m_ResourceManager->BuildResource(&DepthDesc, ResourceFlagPack(), &optClear);
	

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = GetDepthViewDesc();
	Screen.DsvPack = m_ResourceManager->AddDsv(name + "Dsv", Screen.DepthBuffer, &dsvDesc);

	m_ResourceManager->ResourceStateTransition(
		CommandList, Screen.DepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	CmdListManager->ExcuteCommandLists();

	CmdListManager->FlushCommandQueue(m_GCS);
	CmdListManager->Reset(CmdIndex);
	Screen.Viewport.TopLeftX = 0;
	Screen.Viewport.TopLeftY = 0;
	Screen.Viewport.Width = static_cast<float>(set.Width);
	Screen.Viewport.Height = static_cast<float>(set.Height);
	Screen.Viewport.MinDepth = 0.0f;
	Screen.Viewport.MaxDepth = 1.0f;

	Screen.ScissorRect = { 0, 0, set.Width, set.Height };

	m_ScreenDatas[name] = Screen;
}
void ScreenManager::ReadyScreen(const string& name, ID3D12GraphicsCommandList* CmdList)
{
	if (m_ScreenDatas.find(name) == m_ScreenDatas.end())
		return;
	ScreenData data = m_ScreenDatas[name];
	int CurrOffset = data.CurrBackBufferOffset;
	CmdList->RSSetViewports(1, &data.Viewport);
	CmdList->RSSetScissorRects(1, &data.ScissorRect);

	m_ResourceManager->ResourceStateTransition(
		CmdList,
		data.RtvPack[CurrOffset]->RenderResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	CmdList->ClearRenderTargetView(data.RtvPack[CurrOffset]->Handle,
		DirectX::Colors::Black, 0, nullptr);
	CmdList->ClearDepthStencilView(data.DsvPack->Handle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	CmdList->OMSetRenderTargets(1, &data.RtvPack[CurrOffset]->Handle, true, &data.DsvPack->Handle);
}
void ScreenManager::OutputScreen(const string& name, ID3D12GraphicsCommandList* CmdList)
{
	if (m_ScreenDatas.find(name) == m_ScreenDatas.end())
		return;
	ScreenData* data = &m_ScreenDatas[name];
	int CurrOffset = data->CurrBackBufferOffset;
	m_ResourceManager->ResourceStateTransition(
		CmdList,
		data->RtvPack[CurrOffset]->RenderResource,
		D3D12_RESOURCE_STATE_PRESENT);

	m_CmdListManager->ExcuteCommandLists();

	ThrowIfFailed(data->SwapChain->Present(0, 0));
	data->CurrBackBufferOffset = (CurrOffset + 1) % SwapChainBufferCount;

	m_GCS->IncreaseOffset();
	m_GCS->IncreaseOffset();
	m_CmdListManager->Signal(m_GCS->GetFence(), m_GCS->GetOffset());
}
ScreenManager::ScreenData& ScreenManager::GetScreenData(const string& name)
{
	if (m_ScreenDatas.find(name) == m_ScreenDatas.end())
		return dummy;
	return m_ScreenDatas[name];
}
const ScreenManager::ScreenData& ScreenManager::GetScreenData(const string& name) const
{
	if (m_ScreenDatas.find(name) == m_ScreenDatas.end())
		return dummy;
	return m_ScreenDatas.at(name);
}
DXGI_SWAP_CHAIN_DESC ScreenManager::GetSwapChainDesc(const DxSetting& set)
{
	DXGI_SWAP_CHAIN_DESC desc;
	desc.BufferDesc.Width = set.Width;
	desc.BufferDesc.Height = set.Height;
	desc.BufferDesc.RefreshRate.Numerator = 0;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.Format = BackBufferFormat;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = SwapChainBufferCount;
	desc.Windowed = !set.FullScreen;
	desc.OutputWindow = set.hWnd;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	return desc;
}
D3D12_RESOURCE_DESC ScreenManager::GetDepthResourceDesc(const DxSetting& set)
{
	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC desc;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = set.Width;
	desc.Height = set.Height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	return desc;
}
D3D12_DEPTH_STENCIL_VIEW_DESC ScreenManager::GetDepthViewDesc()
{
	D3D12_DEPTH_STENCIL_VIEW_DESC desc;
	desc.Flags = D3D12_DSV_FLAG_NONE;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	desc.Format = DepthBufferFormat;
	desc.Texture2D.MipSlice = 0;
	return desc;
}