#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "Object/JGObject.h"
#include <functional>



template<class T, class ...Args>
class PDelegate
{


	std::function<T(Args...)> Func;

public:
	void Bind(const std::function<T(Args...)>& func)
	{

	}
	void BindSP(PWeakPtr<T> ptr, const std::function<T(Args...)>& func)
	{

	}
	void BindJGObject(PWeakJGObjectPtr<T> ptr, const std::function<T(Args...)>& func)
	{

	}
	void UnBindSP(PSharedPtr<T> ptr)
	{

	}
	void UnBindJGObject(PJGObjectPtr<T> ptr)
	{

	}
};



template<class T, class ...Args>
class PMultiDelegate
{
	PList<std::function<T(Args...)>> Funcs;
	
public:

	void Add(const std::function<T(Args...)>& func)
	{

	}
	void AddRaw(T* ptr, const std::function<T(Args...)>& func)
	{

	}
	void AddSP(PWeakPtr<T> ptr, const std::function<T(Args...)>& func)
	{

	}
	void AddJGObject(PWeakJGObjectPtr<T> ptr, const std::function<T(Args...)>& func)
	{

	}
	void RemoveRaw(T* ptr)
	{

	}
	void RemoveSP(PSharedPtr<T> ptr)
	{

	}
	void RemoveJGObject(PJGObjectPtr<T> ptr)
	{

	}

	bool BroadCast(Args ... args)
	{

		return true;
	}
};