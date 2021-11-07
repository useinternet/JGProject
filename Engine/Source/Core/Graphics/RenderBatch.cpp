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
	u64 RenderBatch::GetCommandID() const
	{
		if (mConnectedRenderer == nullptr)
		{
			return 0;
		}
		return mConnectedRenderer->GetCommandID();
	}
}