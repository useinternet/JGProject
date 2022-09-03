#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "Object/JGObject.h"
#include <functional>

class IDelegateInstanceBase : public IMemoryObject
{
public:
	virtual ~IDelegateInstanceBase() = default;

	virtual bool IsBound() const = 0;
};

template<class Ret, class ...Args>
class IDelegateInstance : public IDelegateInstanceBase
{
public:
	virtual ~IDelegateInstance() = default;

	virtual Ret Execute(Args ... args) = 0;
};

template<class T, class Ret, class ...Args>
class PDelegateInstance : public IDelegateInstance<Ret, Args...>
{
	bool _bIsStatic;

	std::function<Ret(Args...)> _func;
	PWeakPtr<T> _refObject;
public:
	virtual ~PDelegateInstance() = default;

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
		Reset();
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

	static PDelegateInstance<T, Ret, Args ...> Create(const std::function<Ret(Args...)>& func)
	{
		PDelegateInstance<T, Ret, Args ...> result;
		result.Bind(func);

		return result;
	}

	template<class T>
	static PDelegateInstance<T, Ret, Args ...> Create(T* rawPtr, const std::function<Ret(Args...)>& func)
	{
		PDelegateInstance<T, Ret, Args ...> result;
		result.Bind(rawPtr, func);

		return result;
	}

	template<class T>
	static PDelegateInstance<T, Ret, Args ...> Create(PWeakPtr<T> ptr, const std::function<Ret(Args...)>& func)
	{
		PDelegateInstance<T, Ret, Args ...> result;
		result.Bind(ptr, func);

		return result;
	}
};

template<class ... Args>
class PDelegate
{
	PSharedPtr<IDelegateInstance<void, const Args&...>> Instance;

public:
	template<class T>
	void Bind(PWeakPtr<T> ptr, const std::function<void(const Args&...)>& func)\
	{
		Instance = Allocate(PDelegateInstance<T, void, const Args&...>::Create(ptr, func));\
	}
	
	bool IsBound() const\
	{
		if (Instance == nullptr)\
		{
			return false;
		}

		return Instance->IsBound();
	}
	
	void Execute(const Args& ... args)
	{
		if (Instance == nullptr)
		{
			return;
		}
			
		Instance->Execute(args...);
	}
			
	void ExecuteIfBound(const Args& ... args)
	{
		if (Instance->IsBound() == false)\
		{
			return;
		}
			
		Instance->Execute(args...);
	}
			
public:
	template<class T>
	static PDelegate<const Args&...> Create(PWeakPtr<T> ptr, const std::function<void(const Args&...)>& func)\
	{
		PDelegate<const Args&...> delegate;
		delegate.Bind(ptr, func);
		return delegate;
	}
};

#define JG_DECLARE_DELEGATE(DelegateName) \
using DelegateName = PDelegate<>;\

#define JG_DECLARE_DELEGATE_ONEPARAM(DelegateName, OneParam)\
using DelegateName   = PDelegate<##OneParam>;\

#define JG_DECLARE_DELEGATE_TWOPARAM(DelegateName, OneParam, TwoParam) \
using DelegateName   = PDelegate<##OneParam, ##TwoParam>;\

#define JG_DECLARE_DELEGATE_THREEPARAM(DelegateName, OneParam, TwoParam, ThreeParam) \
using DelegateName   = PDelegate<##OneParam, ##TwoParam, ##ThreeParam>; \

#define JG_DECLARE_DELEGATE_FOURPARAM(DelegateName, OneParam, TwoParam, ThreeParam, FourParam) \
using DelegateName   = PDelegate<##OneParam, ##TwoParam, ##ThreeParam, ##FourParam>; \

#define JG_DECLARE_DELEGATE_FIVEPARAM(DelegateName, OneParam, TwoParam, ThreeParam, FourParam, FiveParam) \
using DelegateName   = PDelegate<##OneParam, ##TwoParam, ##ThreeParam, ##FourParam, ##FiveParam>; \