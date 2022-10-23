#pragma once
#include "CoreDefines.h"
#include "String/String.h"

// 임시 기본 Window
#define _JG_PLATFORM_WINDOWS

// Platform 별 Include
// Window
#ifdef _JG_PLATFORM_WINDOWS
#include <Windows.h>
#endif

// Platform 별 Define

#ifdef _JG_PLATFORM_WINDOWS

using HPlatformModule = HMODULE;
using HPlatformInstance = HINSTANCE;

#endif


//
template<class ReturnType, class ...Args>
class HPlatformFunction
{
	friend class HPlatform;
private:
	ReturnType(*func)(Args...);
public:
	ReturnType operator()(Args... args)
	{
		return func(args...);
	}
	bool IsVaild() const {
		return func != nullptr;
	}
};


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


	static HPlatformInstance LoadDll(const PString& dllPath);
	static void UnLoadDll(HPlatformInstance instance);

	template<class Return, class ... Args>
	static HPlatformFunction<Return, Args...> LoadFuncInDll(HPlatformInstance instance, const PString& funcName)
	{
		HPlatformFunction<Return, Args...> result;

		if (instance != nullptr)
		{
			result.func = (Return(*)(Args...))::GetProcAddress(instance, funcName.GetCStr());
		}

		return result;
	}
};