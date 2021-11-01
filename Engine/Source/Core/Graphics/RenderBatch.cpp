#include "pch.h"
#include "RenderBatch.h"

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