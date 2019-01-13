#pragma once
#include"DxCommon/DxCommon.h"
#include"DxSetting.h"

namespace JGRC
{
	class RCORE_EXPORT DxCore
	{
	private:
		bool      mFullscreenState = false;
		DxSetting mSetting;

		bool      m4xMsaaState = false;    // 4X MSAA enabled
		UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA

		Microsoft::WRL::ComPtr<IDXGIFactory4> mFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;

		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		UINT64 mCurrentFence = 0;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

		static const int SwapChainBufferCount = 2;
		int mCurrBackBuffer = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;

		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

		D3D_DRIVER_TYPE md3dDriverType  = D3D_DRIVER_TYPE_HARDWARE;
		DXGI_FORMAT mBackBufferFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		DirectX::XMFLOAT4X4 m_Proj = MathHelper::Identity4x4();
	private:
		DxCore(const DxCore& rhs) = delete;
		DxCore& operator=(const DxCore& rhs) = delete;
	public:
		DxCore(const DxSetting& setting);
		virtual ~DxCore();
		float     AspectRatio()const;

		bool Get4xMsaaState()const;
		void Set4xMsaaState(bool value);
	public:
		void CreateRtvAndDsvDescriptorHeaps();
		void OnResize(const int width, const int height);
		void FlushCommandQueue();
		
	public:
		bool InitDirect3D();
		void StartDraw();
		void EndDraw();
		ID3D12Resource* DepthStencilBuffer() const;
		ID3D12Resource* CurrentBackBuffer()const;
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	public:
		ID3D12Device* Device() const                   { return mDevice.Get(); }
		ID3D12CommandQueue* CommandQueue() const       { return mCommandQueue.Get(); }
		ID3D12GraphicsCommandList* CommandList() const { return mCommandList.Get(); }
		ID3D12CommandAllocator* CommandAllocator() const { return mDirectCmdListAlloc.Get(); }
		ID3D12Fence* Fence() const                     { return mFence.Get(); }
		ID3D12DescriptorHeap* RtvHeap() const          { return mRtvHeap.Get(); }
		ID3D12DescriptorHeap* DsvHeap() const          { return mDsvHeap.Get(); }
		UINT64 CurrentFence() const { return mCurrentFence; }
	public:
		UINT RtvDescriptorSize() const        { return mRtvDescriptorSize; }
		UINT DsvDescriptorSize() const       { return mDsvDescriptorSize; }
		UINT CbvSrvUavDescriptorSize() const { return mCbvSrvUavDescriptorSize; }
		UINT Get4xMsaaQuality() const        { return m4xMsaaQuality; }
		const DirectX::XMFLOAT4X4& Proj() const       { return m_Proj; }
		const DxSetting& GetSettingDesc() const       { return mSetting; }
	private:
		void CreateCommandObjects();
		void CreateSwapChain();

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
	
	};
}
