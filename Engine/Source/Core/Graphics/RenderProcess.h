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
		struct AwakeData
		{
			Renderer* pRenderer = nullptr;

			AwakeData() = default;
		};
		struct ReadyData
		{
			Renderer* pRenderer = nullptr;
			Graphics::RenderPassData* pRenderPassData = nullptr;
			SharedPtr<IGraphicsContext> GraphicsContext = nullptr;
			SharedPtr<IComputeContext>  ComputeContext = nullptr;
			RenderInfo Info;

			ReadyData() = default;
		};
		struct RunData
		{
			Renderer* pRenderer = nullptr;
			SharedPtr<IGraphicsContext> GraphicsContext = nullptr;
			SharedPtr<IComputeContext>  ComputeContext = nullptr;

			RenderInfo Info;
			SharedPtr<RenderResult> Result;

			RunData() = default;
		};
	public:
		virtual void Awake(const AwakeData& data) = 0;
		virtual void Ready(const ReadyData& data) = 0;
		virtual void Run(const RunData& data) = 0;
		virtual bool IsCompelete() = 0;
		virtual Type GetType() const = 0;
	};
}