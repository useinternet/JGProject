#pragma once
#include "JGCore.h"
#include "JGGraphics.h"


namespace JG
{
	class IGraphicsAPI;
	class Renderer;
	class RenderInfo;




	class IRenderProcess
	{

	public:
		virtual void Awake(Renderer* renderer) = 0;
		virtual void Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info) = 0;
		virtual void Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result) = 0;
		virtual bool IsCompelete() = 0;
		virtual Type GetType() const = 0;
	};
}