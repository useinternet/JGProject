#include"pch.h"
#include"GraphicsPass.h"
#include"Renderer.h"

namespace GR
{
	namespace Dx12
	{
		Renderer* GraphicsPass::ms_Renderer = nullptr;
		Renderer* GraphicsPass::GetRenderer() {
			return ms_Renderer;
		}
	}
}