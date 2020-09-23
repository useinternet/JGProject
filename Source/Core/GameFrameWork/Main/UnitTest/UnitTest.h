#pragma once


#include "GFWIF.h"


namespace GFW
{
	class UnitTest
	{

	public:
		virtual void Start()          = 0;
		virtual void Tick(float tick) = 0;
		virtual void Destroy()        = 0;
		virtual uint64_t GetID() = 0;
		virtual ~UnitTest() {}
	};
}