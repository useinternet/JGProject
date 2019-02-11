#pragma once

#include"DxCommon/DxCommon.h"
#include"DxSetting.h"
namespace JGRC
{
	class CommandListManager;
	class GpuCpuSynchronizer;
	class ScreenManager
	{
		static const int SwapChainBufferCount = 2;
		static const DXGI_FORMAT BackBufferFormat  = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT DepthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	public:
		struct ScreenData
		{
			ID3D12Resource* Buffer[SwapChainBufferCount];
			ID3D12Resource* DepthBuffer;
			Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
			RenderTargetPack*     RtvPack[SwapChainBufferCount];
			DepthStencilViewPack* DsvPack;
			int CurrBackBufferOffset = 0;


			D3D12_VIEWPORT Viewport;
			D3D12_RECT     ScissorRect;
		};
	private:
		std::unordered_map<std::string, ScreenData> m_ScreenDatas;
		bool m4xMsaaState = false;
		IDXGIFactory4*   m_Factory         = nullptr;
		ResourceManager* m_ResourceManager = nullptr;
		CommandListManager* m_CmdListManager = nullptr;
		GpuCpuSynchronizer* m_GCS = nullptr;
	public:
		void CreateManager(IDXGIFactory4* factory, ResourceManager* manager, GpuCpuSynchronizer* GCS,
			CommandListManager* CmdListManager);
		void AddSwapChain(
			const std::string& name,
			HWND hWnd,
			const DxSetting& set,
			CommandListManager* CmdListManager,
			UINT CmdIndex);
		void ReadyScreen(const std::string& name, ID3D12GraphicsCommandList* CmdList);
		void OutputScreen(const std::string& name, ID3D12GraphicsCommandList* CmdList);
		ScreenData& GetScreenData(const std::string& name);
		const ScreenData& GetScreenData(const std::string& name) const;
	private:
		DXGI_SWAP_CHAIN_DESC GetSwapChainDesc(const DxSetting& set);
		D3D12_RESOURCE_DESC GetDepthResourceDesc(const DxSetting& set);
		D3D12_DEPTH_STENCIL_VIEW_DESC GetDepthViewDesc();
	};
}