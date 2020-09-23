#pragma once


#include "GEObject.h"


namespace GE
{
	class GRAPHICSENGINE_API Pass : public GEObject
	{
	public:
		virtual ~Pass() {}
	};

	class GRAPHICSENGINE_API GraphicPass : public Pass
	{
	public:
		virtual ~GraphicPass() {}
	};

	class GRAPHICSENGINE_API ComputePass : public Pass
	{


	public:
		virtual ~ComputePass() {}
	};

}