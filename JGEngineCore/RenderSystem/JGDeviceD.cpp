#include"JGDeviceD.h"
#include"VideoDeviceInfo.h"
#include"../EngineStatics/JGLog.h"
using namespace std;
JGDeviceD::JGDeviceD()
{
	m_VideoDeviceInfomation = make_unique<VideoDeviceInfo>();
}
JGDeviceD::~JGDeviceD()
{
	//Microsoft::WRL::ComPtr<ID3D11Debug> DebugDevice;
	//m_Device->QueryInterface(__uuidof(ID3D11Debug), (void**)DebugDevice.GetAddressOf());
	//DebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

}
bool JGDeviceD::CreateDevice()
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
	D3D_FEATURE_LEVEL HighestLevel;


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
		JGLog::Write(ELogLevel::Error, TT("D3D11Device Create Failed."));
		return false;
	}
	if (HighestLevel != D3D_FEATURE_LEVEL_11_0)
	{
		JGLog::Write(ELogLevel::Warning, TT("D3DFeatureLevel is not 'Level11'"));
		// 예외처리 로그 출력
	}
	return true;
}
ID3D11Device* JGDeviceD::GetDevice()
{
	if (m_Device.Get())
	{
		return m_Device.Get();
	}
	// 오류 출력

	JGLog::Write(ELogLevel::Error, TT("D3DDevice is nullptr"));
	return nullptr;
}
ID3D11DeviceContext* JGDeviceD::GetContext()
{
	if (m_DeviceContext.Get())
	{
		return m_DeviceContext.Get();
	}
	// 오류 출력
	JGLog::Write(ELogLevel::Error, TT("D3DDeviceContext is nullptr"));
	return nullptr;
}
VideoDeviceInfo* JGDeviceD::GetVideoDeviceInformation()
{
	if (m_VideoDeviceInfomation.get())
	{
		return m_VideoDeviceInfomation.get();
	}

	// 오류 출력
	JGLog::Write(ELogLevel::Warning, TT("VideoDeviceInformation is nullptr"));
	return nullptr;
}
