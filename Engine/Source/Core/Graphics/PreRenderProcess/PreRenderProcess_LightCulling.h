#pragma once
#include "Graphics/RenderProcess.h"






namespace JG
{
	class IComputer;
	class IReadBackBuffer;


	class PreRenderProcess_LightCulling : public IRenderProcess
	{
	private:
		List<SharedPtr<IComputer>>       mComputers;
		List<SharedPtr<IReadBackBuffer>> mReadBackBuffers;
	public:
		PreRenderProcess_LightCulling();
	public:
		virtual void Run(Renderer* renderer,IGraphicsAPI* api, const RenderInfo& info) override;
		virtual bool IsCompelete() override;
	};
}