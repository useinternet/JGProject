#pragma once
#include "JGCore.h"



namespace JG
{
	class IGraphicsAPI;
	class RenderInfo;
	class IRenderProcess
	{
	public:
		virtual void Run(IGraphicsAPI* api, const RenderInfo& info) = 0;
		virtual bool IsCompelete() = 0;
	};
}