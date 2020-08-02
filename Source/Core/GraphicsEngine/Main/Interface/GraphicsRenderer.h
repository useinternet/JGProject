#pragma once
#include "GEObject.h"



/*
class : GraphicsRenderer

ex)


*/





namespace GE
{

	class Scene;
	class Material;
	class GRAPHICSENGINE_API GraphicsRenderer : public GEObject
	{
	public:
		GraphicsRenderer() {}
		virtual ~GraphicsRenderer() {}
		virtual void BeginFrame() = 0;
		virtual bool DrawCall(GE::Scene* scene) = 0;
		virtual bool CompileMaterial(GE::Material* mat) = 0;
		virtual void EndFrame() = 0;

	};
}