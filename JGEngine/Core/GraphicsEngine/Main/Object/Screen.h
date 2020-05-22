#pragma once
#include "GObject.h"





namespace GE
{
	class GraphicsEngine;
	class GRAPHICSENGINE_API Screen : public GObject
	{
		friend GraphicsEngine;
	protected:
		virtual void Create(HWND hWnd, int width, int height, _FORMAT format, _ScreenFlags = ScreenFlag_None) = 0;






	};




}