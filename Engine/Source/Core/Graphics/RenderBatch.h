#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"
namespace JG
{
	class Renderer;
	class RenderInfo;
	class RenderBatch
	{
	
		Renderer* mConnectedRenderer = nullptr;
	public:
		RenderBatch() = default;
		virtual ~RenderBatch() = default;
	public:
		void ConnectRenderer(Renderer* renderer);
		Renderer* GetConnectedRenderer() const;

		u64 GetCommandID() const;
	protected:
		friend Renderer;
		virtual bool Begin(const RenderInfo & info) = 0;
		virtual void End() = 0;
	};


}