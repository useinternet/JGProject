#pragma once
#include"../EngineStatics/Engine.h"



/* 
Class : JGDeviceD
@m ComPtr<ID3D11Device>              m_Device                : D3D 장치
@m ComPtr<ID3D11DeviceContext>       m_DeviceContext         : D3D 장치 Context
@m unique_ptr<class VideoDeviceInfo> m_VideoDeviceInfomation : 비디오 장치 정보
*/
class ENGINE_EXPORT JGDeviceD
{
private:
	Microsoft::WRL::ComPtr<ID3D11Device>        m_Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;
	std::unique_ptr<class VideoDeviceInfo>      m_VideoDeviceInfomation;
public:
	JGDeviceD();
	~JGDeviceD();
	/* 
	Exp : 장치를 생성한다.*/
	bool CreateDevice();
	/*
	Exp : ID3D11Device 포인터를 취득한다 */
	ID3D11Device* GetDevice();
	/*
	Exp : ID3D11DeviceContext 포인터를 취득한다 */
	ID3D11DeviceContext* GetContext();
	/*
	Exp : 비디오 장치 정보를 취득한다 */
	VideoDeviceInfo* GetVideoDeviceInformation();

	/*
	Exp : 인스턴스를 가져온다.*/
	static JGDeviceD* GetInstance();
};





