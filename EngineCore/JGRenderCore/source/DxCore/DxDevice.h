#pragma once
#include"DxCommon/DxCommon.h"
#include"SceneConfig.h"

namespace JGRC
{
	class RCORE_EXPORT DxDevice
	{
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
		Microsoft::WRL::ComPtr<ID3D12Device>  m_Device;

		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

		D3D_DRIVER_TYPE md3dDriverType  = D3D_DRIVER_TYPE_HARDWARE;
	private:
		DxDevice(const DxDevice& rhs) = delete;
		DxDevice& operator=(const DxDevice& rhs) = delete;
	public:
		DxDevice() = default;
		bool CreateDevice();
	public:
		ID3D12Device*  Get() const  { return m_Device.Get(); }
		IDXGIFactory4* Factory() const { return m_Factory.Get(); }
	public:
		UINT RtvDescriptorSize() const        { return mRtvDescriptorSize; }
		UINT DsvDescriptorSize() const       { return mDsvDescriptorSize; }
		UINT CbvSrvUavDescriptorSize() const { return mCbvSrvUavDescriptorSize; }
	private:
		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
	};
}
