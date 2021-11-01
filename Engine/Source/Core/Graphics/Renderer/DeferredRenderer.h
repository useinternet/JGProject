#pragma once
#include "Graphics/Renderer.h"



namespace JG
{
	class DeferredRenderer : public Renderer
	{
	public:
		DeferredRenderer();
		virtual ~DeferredRenderer() = default;
	public:
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::Deferred; }
		virtual int ArrangeObject(const ObjectInfo & info) override;
	private:
		void Draw(int objectType, const List<ObjectInfo>&objectList);
	};
}