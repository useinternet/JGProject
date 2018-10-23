#pragma once
#include"Common/JGRCCommon.h"



namespace JGRenderCore
{
	class CORE_EXPORT JGDevice
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11Device>        m_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;
	public:
		JGDevice();
		~JGDevice();
		/*
		Exp : 장치를 생성한다.*/
		bool CreateDevice();
		/*
		Exp : ID3D11Device 포인터를 취득한다 */
		ID3D11Device* GetDevice();
		/*
		Exp : ID3D11DeviceContext 포인터를 취득한다 */
		ID3D11DeviceContext* GetContext();
	};


}






