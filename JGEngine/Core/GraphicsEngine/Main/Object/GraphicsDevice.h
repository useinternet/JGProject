#pragma once
#include "GObject.h"



namespace GE
{
	class GRAPHICSENGINE_API GraphicsDevice : public GObject
	{
	public:
		virtual void Init()     = 0;
		virtual bool Load()     = 0;
		virtual void Update()   = 0;
		virtual void Destroy()  = 0;




	};




}