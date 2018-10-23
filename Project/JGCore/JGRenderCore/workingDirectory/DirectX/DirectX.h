#pragma once
#include"Common/JGRCCommon.h"

namespace JGRenderCore
{
	class CORE_EXPORT DirectX
	{
	private:
		std::unique_ptr<class JGDevice>       m_Device;
		std::unique_ptr<class JGSwapChain>    m_SwapChain;
		std::unique_ptr<class JGRenderTarget> m_RenderTarget;
		std::unique_ptr<class JGRenderState>  m_RenderState;
	public:
		DirectX();
		~DirectX();
	};


}
