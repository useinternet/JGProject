#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "Object/JGObject.h"
#include <functional>

// ¿Ã∏ß ∞ÌπŒ
class PDelegateInstanceBase : public IMemoryObject
{
public:
	virtual ~PDelegateInstanceBase() = default;

	virtual bool IsBound() const = 0;
};

template<class Ret, class ...Args>
class PFunctionInstance : public PDelegateInstanceBase
{
public:
	virtual ~PFunctionInstance() = default;

	virtual Ret Execute(Args ... args) = 0;
};

template<class T, class Ret, class ...Args>
class PSingleDelegateInstance : public PFunctionInstance<Ret, Args...>
{
	bool _bIsStatic;

	std::function<Ret(Args...)> _func;
	PWeakPtr<T> _refObject;
public:
	virtual ~PSingleDelegateInstance() = default;

public:
	void Bind(const std::function<Ret(Args...)>& func)
	{
		Reset();
		_func = func;
		_bIsStatic = true;
	}

	void Bind(T* rawPtr, const std::function<Ret(Args...)>& func)
	{
		Bind(SharedWrap<T>(rawPtr), func);
	}

	void Bind(PWeakPtr<T> ptr, const std::function<Ret(Args...)>& func)
	{
		_refObject = ptr;
		_func      = func;
		_bIsStatic = false;
	}

	virtual Ret Execute(Args ... args) override
	{
		return _func(args...);
	}

	virtual bool IsBound() const override
	{
		if (_func == nullptr)
		{
			return false;
		}

		if (_bIsStatic == false && _refObject.IsValid() == false)
		{
			return false;
		}
	
		return true;
	}

	void Reset()
	{
		_func = nullptr;
		_bIsStatic = false;
		_refObject.Reset();
		_refObject = nullptr;
	}

	static PSingleDelegateInstance<T, Ret, Args ...> Create(const std::function<Ret(Args...)>& func)
	{
		PSingleDelegateInstance<T, Ret, Args ...> result;
		result.Bind(func);

		return result;
	}

	template<class T>
	static PSingleDelegateInstance<T, Ret, Args ...> Create(T* rawPtr, const std::function<Ret(Args...)>& func)
	{
		PSingleDelegateInstance<T, Ret, Args ...> result;
		result.Bind(rawPtr, func);

		return result;
	}

	template<class T>
	static PSingleDelegateInstance<T, Ret, Args ...> Create(PWeakPtr<T> ptr, const std::function<Ret(Args...)>& func)
	{
		PSingleDelegateInstance<T, Ret, Args ...> result;
		result.Bind(ptr, func);

		return result;
	}
};

//using PMultiDelegateHandle = uint64;
//
//template<class ...Args>
//class PMultiDelegate
//{
//	using PSingleDelegate = PDelegate<void, Args...>
//
//	PList<PSharedPtr<PSingleDelegate>> _singleDelegates;
//public:
//	void Add(const std::function<T(Args...)>& func)
//	{
//		_singleDelegates.push_back(Allocate<PSingleDelegate>(PSingleDelegate::Create(func)));
//	}
//
//	void Add(T* ptr, const std::function<T(Args...)>& func)
//	{
//		if (ptr == nullptr)
//		{
//			return;
//		}
//
//		Bind(Wrap<T>(ptr), func);
//	}
//
//	void Add(PWeakPtr<T> ptr, const std::function<T(Args...)>& func)
//	{
//		_singleDelegates.push_back(Allocate<PSingleDelegate>(PSingleDelegate::Create(ptr, func)));
//	}
//
//	void RemoveAll(T* ptr)
//	{
//		if (ptr == nullptr)
//		{
//			return;
//		}
//	}
//
//	void Clear()
//	{
//	
//	}
//
//};