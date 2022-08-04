#pragma once


#include "CoreDefines.h"



class PMath
{
public:
	template<class T>
	static T Align(T value, T alignment)
	{
		T mask = alignment - 1;

		return value + (-value & mask);
	}




};