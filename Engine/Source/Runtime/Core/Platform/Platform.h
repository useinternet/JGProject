#pragma once
#include "CoreDefines.h"
#include "PlatformDefines.h"
#include "String/String.h"


template<class ReturnType, class ...Args>
class HPlatformFunction
{
	friend class HPlatform;
private:
	ReturnType(*func)(Args...);
public:
	HPlatformFunction() = default;
	HPlatformFunction(ReturnType(*inFunc)(Args...)) : func(inFunc) {}
	ReturnType operator()(Args... args)
	{
		return func(args...);
	}
	bool IsVaild() const {
		return func != nullptr;
	}
};

class PJWindow;
class HGuid;
struct HJWindowArguments;
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

	// -- Dll -- 
	static HJInstance LoadDll(const PString& dllPath);
	static void UnLoadDll(HJInstance instance);

	template<class Return, class ... Args>
	static HPlatformFunction<Return, Args...> LoadFuncInDll(HJInstance instance, const PString& funcName)
	{
		HPlatformFunction<Return, Args...> result;

		if (instance != nullptr)
		{
			result.func = (Return(*)(Args...))::GetProcAddress(instance, funcName.GetCStr());
		}

		return result;
	}
	//


	// -- Windows --
	static PSharedPtr<PJWindow> CreateJWindow(const HJWindowArguments& args);
	static HGuid  NewGuid();
	static uint64 GuidHash(const HGuid& hash);
};