#pragma once
#include "CoreDefines.h"




class HPlatform
{
public:
	template<class T, class ... Args>
	static T* Allocate(Args ... args)
	{
		return new T(args...);
	}

	template<class T>
	static void Deallocate(T*& inPtr)
	{
		if (inPtr == nullptr)
		{
			return;
		}

		delete inPtr;
		inPtr = nullptr;
	}
};