#pragma once
#include "JGCore.h"



namespace JG
{
	class IGraphicsAPI;
	class Renderer;
	class RenderInfo;
	class IRenderProcess
	{
	public:
		virtual void Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info) = 0;
		virtual bool IsCompelete() = 0;
	};
}