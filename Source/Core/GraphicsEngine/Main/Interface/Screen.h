#pragma once
#include "GEObject.h"

class GraphicsIF;
namespace GE
{
	class Scene;
	class GRAPHICSENGINE_API Screen
	{
		friend GraphicsIF;
	public:
		
		virtual void  RequestResize(const JVector2Int& size) = 0;
		virtual void  BindScene(Scene* scene) = 0;
		virtual void  UnBindScene() = 0;


		virtual const JVector2Int& GetSize() const = 0;


		virtual HWND  GetHWND() const = 0;

		virtual ~Screen() {};

	private:
		virtual void Init(HWND hWnd, int width, int height) = 0;
	};
}