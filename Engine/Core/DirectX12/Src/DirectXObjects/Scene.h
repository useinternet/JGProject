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

	class Scene
	{
	private:
		GraphicsPSO  m_ScreenPSO;
		RenderTarget m_RenderTarget;
		Viewport     m_Viewport;
		ScissorRect  m_ScissorRect;
		bool m_ChangedDebug;
	public:
		Scene(int width, int height);
	public:
		void  DebugModeOn(int GbufferSlot);
		void  DebugModeOff();

		void  ReSize(int width, int height);
		void  Draw(CommandList* commandList,GBuffer* gbuffer);
		const Texture& GetTexture();
	};
}