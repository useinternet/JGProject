#pragma once
#include"Common/JGRCCommon.h"

namespace JGRenderCore
{
	class CORE_EXPORT JGSwapChain
	{
	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
	public:
		JGSwapChain();
		~JGSwapChain();
		/*
		Exp : 스왑체인을 생성한다.
		@param ID3D11Device* Device : Direct 장치
		@param HWND hWnd : 윈도우 핸들
		@param const bool bFullScreen : 전체 화면 여부
		@param const int ScreenWidth  : 스크린 가로 길이
		@param const int ScreenHeight : 스크린 세로 길이
		*/
		bool CreateSwapChain(ID3D11Device* Device, HWND hWnd, const bool bFullScreen,
			const int ScreenWidth, const int ScreenHeight);

		/*
		Exp : 스왑체인 포인터를 취득한다.*/
		IDXGISwapChain* Get();
	private:
	};
}

