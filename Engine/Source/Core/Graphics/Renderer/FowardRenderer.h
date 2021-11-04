#pragma once
#include "Graphics/Renderer.h"



namespace JG
{
	class FowardRenderer : public Renderer
	{
	public:
		FowardRenderer();
		virtual ~FowardRenderer() = default;
	public:
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::Foward; }
		virtual int ArrangeObject(const ObjectInfo& info) override;
	private:
		void Ready(IGraphicsAPI* api, const RenderInfo& info);
		void Draw(int objectType, const List<ObjectInfo>& objectList);


	};
}