#pragma once
#include"../EngineStatics/Engine.h"


typedef DXGI_ADAPTER_DESC1          AdapterDesc;
typedef DXGI_MODE_DESC              DisplayModeDesc;

/*
Class : VideoDeviceInfo
@m AdapterDesc     m_AdaterDesc : 비디오 카드
@m DisplayModeList m_ModeList   : 디스플레이 모드 목록
*/
class ENGINE_EXPORT VideoDeviceInfo
{
	typedef std::vector<DisplayModeDesc> DisplayModeList;
private:
	AdapterDesc     m_AdaterDesc;
	DisplayModeList m_ModeList;
public:
	/*
	Exp : 비디오장치 정보를 탐색합니다.*/
	bool SearchVideoDeviceInformation();
	/*
	Exp : 비디오 카드 정보를 가져옵니다.*/
	AdapterDesc& GetVideoCard();
	/*
	Exp : 디스플레이 모드 목록에서 화면에 맞는 모드 정보를 가져옵니다. */
	DisplayModeDesc& GetDisplayModeList(const int ScreenWidth, const int ScreenHeight);
};
