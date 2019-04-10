#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"DirectXToolKit/Texture.h"
#include"DirectXToolKit/RenderTarget.h"
#include"DirectXToolKit/Viewport.h"
#include"DirectXToolKit/ScissorRect.h"
namespace Dx12
{
	class Dx12Object;
	class CommandList;
	struct PassCB;
	typedef int EGBufferTexture;

	namespace GBufferTexture
	{
		/*
		|  R  |  G  |  B  |
RTV1    |      alebdo     |  DXGI_FORMAT_R8G8B8
RTV2    |      normal     |  DXGI_FORMAT_R8G8B8
RTV3    |     specular    |  DXGI_FORMAT_R8G8B8
RTV4    |depth|     |     |  DXGI_FORMAT_R8
		*/
		enum
		{
			Albedo = 0,
			Normal,
			Specular,
			Depth,
			NumBuffer
		};
	}

	class GBuffer
	{
	private:
		static const DXGI_FORMAT ms_AlebdoFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT ms_NormalFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT ms_SpecularFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT ms_DepthFormat    = DXGI_FORMAT_R8_UNORM;
		static std::vector<DXGI_FORMAT> ms_RtvFormatArray;
	public:
		static const std::vector<DXGI_FORMAT>& GetRtvFormatArray();
	private:
		RenderTarget m_RenderTarget;
		Viewport     m_Viewport;
		ScissorRect  m_ScissorRect;
	public:
		GBuffer(int width, int height);

		void Resize(int width, int height);
		void Draw(
			CommandList* commandList,
			const std::vector<Dx12Object*>& objArray, 
			const PassCB& passcb);

		const Texture& GetTexture(EGBufferTexture gt);
	};

}