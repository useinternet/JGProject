#include"JGDevice.h"
using namespace JGRenderCore;
using namespace std;
JGDevice::JGDevice()
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
}
JGDevice::~JGDevice() {}
bool JGDevice::CreateDevice()
{
	UINT CreateDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#else
	CreateDeviceFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
#endif 

	HRESULT result = S_OK;

	const size_t LevelCount = 6;
	D3D_FEATURE_LEVEL FeatureLevel[LevelCount] =
	{
		D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1
	};
	D3D_FEATURE_LEVEL HighestLevel = D3D_FEATURE_LEVEL_11_0;


	// 장치 생성
	result = D3D11CreateDevice(nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, nullptr,
		CreateDeviceFlags, 
		FeatureLevel, LevelCount,
		D3D11_SDK_VERSION, 
		m_Device.GetAddressOf(),
		&HighestLevel,
		m_DeviceContext.GetAddressOf()
	);
	if (FAILED(result))
	{
		return false;
	}
	if (HighestLevel != D3D_FEATURE_LEVEL_11_0)
	{
		// 예외처리 로그 출력
	}
	return true;
}
ID3D11Device* JGDevice::GetDevice()
{
	if (m_Device.Get())
	{
		return m_Device.Get();
	}
	// 오류 출력
	return nullptr;
}
ID3D11DeviceContext* JGDevice::GetContext()
{
	if (m_DeviceContext.Get())
	{
		return m_DeviceContext.Get();
	}
	// 오류 출력
	return nullptr;
}

