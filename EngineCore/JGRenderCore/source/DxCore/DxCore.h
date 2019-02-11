#pragma once
#include"DxCommon/DxCommon.h"
#include"DxSetting.h"

namespace JGRC
{
	class RCORE_EXPORT DxCore
	{
	private:
		DxSetting mSetting;
		Microsoft::WRL::ComPtr<IDXGIFactory4> mFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;


		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

		D3D_DRIVER_TYPE md3dDriverType  = D3D_DRIVER_TYPE_HARDWARE;
	private:
		DxCore(const DxCore& rhs) = delete;
		DxCore& operator=(const DxCore& rhs) = delete;
	public:
		DxCore(const DxSetting& setting);
		virtual ~DxCore();
	public:
		bool InitDirect3D();
	public:
		ID3D12Device* Device() const                   { return mDevice.Get(); }
		IDXGIFactory4* Factory() const { return mFactory.Get(); }
	public:
		UINT RtvDescriptorSize() const        { return mRtvDescriptorSize; }
		UINT DsvDescriptorSize() const       { return mDsvDescriptorSize; }
		UINT CbvSrvUavDescriptorSize() const { return mCbvSrvUavDescriptorSize; }
		const DxSetting& GetSettingDesc() const       { return mSetting; }
	private:
		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
	};
}
