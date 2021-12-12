#include "pch.h"
#include "RenderBatch.h"
#include "Renderer.h"
namespace JG
{
	void RenderBatch::ConnectRenderer(Renderer* renderer)
	{
		mConnectedRenderer = renderer;
	}
	Renderer* RenderBatch::GetConnectedRenderer() const
	{
		return mConnectedRenderer;
	}
}