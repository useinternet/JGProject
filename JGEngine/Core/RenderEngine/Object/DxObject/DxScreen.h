#pragma once


#include "DxObject.h"


namespace RE
{
	class Texture;
	class CommandList;
	struct RENDERENGINE_API DxScreenDesc
	{
		HWND hWnd;
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		uint32_t width = 800;
		uint32_t height = 600;
		uint32_t bufferCount = 3;
	};
	class RENDERENGINE_API DxScreen : public DxObject
	{
	public:
		DxScreen(const std::string& name = "DxScreen") : DxObject(name) {}
		void Init(const DxScreenDesc& desc = DxScreenDesc());
		void SetFinalTexture(const Texture& texture);
		void Update(CommandList* cmdList);
		void Present();
		void ReSize(uint32_t width, uint32_t height);
		HWND GetHandle() const {
			return m_Desc.hWnd;
		}
		const DxScreenDesc& GetDesc() const
		{
			return m_Desc;
		}
	private:
		void UpdateBackBuffer();
	
	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
		std::vector<std::shared_ptr<Texture>>   m_BackBuffer;
		std::shared_ptr<Texture>                m_FinalTexture;
		DxScreenDesc m_Desc;
		/*
		스왑체인
		버퍼 카운트
		크기
		윈도우 핸들
		format
		최종 텍스쳐

		*/
	};
}