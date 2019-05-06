#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"DirectXToolKit/PSO.h"
#include"DirectXToolKit/RenderTarget.h"
#include"DirectXToolKit/Viewport.h"
#include"DirectXToolKit/ScissorRect.h"

namespace Dx12
{
	class Screen;
	class GBuffer;
	class Camera;
	class Scene
	{
	private:
		GraphicsPSO  m_ScenePSO;
		RenderTarget m_RenderTarget;
		Viewport     m_Viewport;
		ScissorRect  m_ScissorRect;
		DXGI_FORMAT  m_Format;
	public:
		Scene(int width, int height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
		Scene(int width, int height, const RenderTarget& rendertarget, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	public:
		void  DebugModeOn(int GbufferSlot);
		void  DebugModeOff();
		void  ReSize(int width, int height);
		void  Draw(CommandList* commandList,GBuffer* gbuffer);
		const Texture& GetTexture() const;
		const RenderTarget& GetRenderTarget() const;
		RenderTarget& GetRenderTarget();
		
	};
}