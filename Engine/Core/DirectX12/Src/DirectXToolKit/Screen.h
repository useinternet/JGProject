#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"RenderTarget.h"
#include"Texture.h"


namespace Dx12
{
	class Screen
	{
		static const int BackBufferCount = 3;
	private:
		HWND m_hWnd;
		Texture  m_BackBuffer[BackBufferCount];
		uint64_t m_FrameValues[BackBufferCount];
		uint64_t m_FenceValues[BackBufferCount];
		ComPtr<IDXGISwapChain4> m_SwapChain;
		uint32_t m_Width;
		uint32_t m_Height;
		DXGI_FORMAT m_Format;
	public:
		Screen() = default;
		~Screen() = default;
	public:
		// 스크린 초기화
		// @param hwnd : 윈도우 핸들
		// @param width, height : 스크린 크기
		// @param format : 스크린 Format
		void InitScreen(HWND hWnd, uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);


		// 화면 크기 재설정
		// @param width,height : 스크린 크기
		void Resize(uint32_t width, uint32_t height);


		// 화면 스왑 
		// @param texture : 스크린에 보여줄 텍스쳐
		// @param clearColor : 텍스쳐의 기본 바탕 색
		void Present(const Texture& texture = Texture(), const DirectX::XMFLOAT4& clearColor = { 0.0f,0.0f,0.0f,1.0f });

		// 화면 비율 가져오기
		float AspectRadio() const;

		// 화면 가로 크기 가져오기
		uint32_t GetWidth() const;

		// 화면 세로 크기 가져오기 
		uint32_t GetHeight() const;

		// 화면 Format 가져오기
		DXGI_FORMAT GetFormat() const;
	private:
		void UpdateRenderTarget();
		void CreateSwapChain();
	};
}