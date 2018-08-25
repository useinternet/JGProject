#include"VideoDeviceInfo.h"
using namespace Microsoft::WRL;


bool VideoDeviceInfo::SearchVideoDeviceInformation()
{
	HRESULT result = S_OK;
	ComPtr<IDXGIFactory1> Factory(nullptr);
	ComPtr<IDXGIAdapter1> Adapter(nullptr);
	ComPtr<IDXGIOutput>   Output(nullptr);

	// DXGIFactory1 생성
	result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)Factory.GetAddressOf());
	if (FAILED(result))
	{
		//예외 로그출력
		return false;
	}


	bool bFind = false;
	// VideoCard 찾기
	for (UINT i = 0; Factory->EnumAdapters1(i, Adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		// Display 찾기
		for (UINT j = 0; Adapter->EnumOutputs(j, Output.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++j)
		{
			UINT modesNum = 0;
			// 디스플레이 모드 찾기
			for (UINT k = 0; k <= DXGI_FORMAT_BC7_UNORM_SRGB; ++k)
			{
				DXGI_FORMAT format = static_cast<DXGI_FORMAT>(k);

				Output->GetDisplayModeList(format, 0, &modesNum, nullptr);

				if (modesNum == 0) continue;

				// 정보 얻어오기
				m_ModeList.reserve(modesNum);
				Output->GetDisplayModeList(format, 0, &modesNum, &m_ModeList[0]);
				Adapter->GetDesc1(&m_AdaterDesc);


				// 찾았으면 Loop break
				bFind = true;
				break;
			}
		}
	}
	// 찾지 못했으면 false 반환
	if (!bFind)
	{
		// 예외 로그 출력
		return false;
	}
	// 성공 로그 출력
	return true;
}

AdapterDesc& VideoDeviceInfo::GetVideoCard()
{
	return m_AdaterDesc;
}

DisplayModeDesc& VideoDeviceInfo::GetDisplayModeList(const int ScreenWidth, const int ScreenHeight)
{
	for (size_t i = 0; i < m_ModeList.size(); ++i)
	{
		if (m_ModeList[i].Width == ScreenWidth && m_ModeList[i].Height == ScreenHeight)
		{
			return m_ModeList[i];
		}
	}

	// 오류 로그 출력
	return m_ModeList.at(0);
}
