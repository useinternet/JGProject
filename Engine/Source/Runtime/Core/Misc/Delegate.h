#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "Object/JGType.h"
#include "Object/JGObject.h"
#include <functional>

class IDelegateInstanceBase : public IMemoryObject
{
public:
	virtual ~IDelegateInstanceBase() = default;

	virtual bool IsBound() const   = 0;
	virtual JGType GetDelegateInstanceType() const = 0;
};

template<class Ret, class ...Args>
class IDelegateInstance : public IDelegateInstanceBase
{
public:
	virtual ~IDelegateInstance() = default;

	virtual Ret Execute(Args ... args) = 0;

	virtual JGType GetDelegateInstanceType() const override
	{
		return JGType::GenerateType<IDelegateInstance<Ret, Args...>>();
	}
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
	PSharedPtr<IDelegateInstance<void, Args...>> Instance;

public:
	template<class T>
	void Bind(PWeakPtr<T> ptr, const std::function<void(Args...)>& func)\
	{
		Instance = Allocate(PDelegateInstance<T, void, Args...>::Create(ptr, func));\
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
		if (Instance == nullptr)
		{
			return;
		}

		if (Instance->IsBound() == false)
		{
			return;
		}
			
		Instance->Execute(args...);
	}
			
public:
	template<class T>
	static PDelegate<Args...> Create(PWeakPtr<T> ptr, const std::function<void(Args...)>& func)\
	{
		PDelegate<Args...> delegate;
		delegate.Bind(ptr, func);
		return delegate;
	}
};

template<class ... Args>
class PMultiDelegate
{
	HList<PDelegate<Args...>> _delegates;
	HList<void*> _delegateKeys;
public:
	template<class T>
	void Add(PWeakPtr<T> ptr, const std::function<void(Args...)>& func)\
	{
		_delegates.push_back(PDelegate<Args...>::Create(ptr, func));
		_delegateKeys.push_back((void*)(ptr.Pin().GetRawPointer()));
	}

	void Remove(void* ptr)
	{
		if (ptr == nullptr)
		{
			return;
		}

		for (uint64 i = 0; i < _delegates.size();)
		{
			if (_delegates[i].IsBound() == false)
			{
				removeInternal(i);
			}
			else if (_delegateKeys[i] == ptr)
			{
				removeInternal(i);
			}
			else ++i;
		}
	}

	void BroadCast(Args... args)
	{
		for (uint64 i = 0; i < _delegates.size();)
		{
			if (_delegates[i].IsBound() == false)
			{
				removeInternal((int32)i);
			}
			else
			{
				_delegates[i].Execute(args...);
				++i;
			}
		}
	}

private:
	void removeInternal(int32 index)
	{
		uint64 len = _delegates.size();

		PDelegate<Args...> tempDelegate = _delegates[len - 1];

		_delegates[len - 1] = _delegates[index];
		_delegates[index] = tempDelegate;
		_delegates.pop_back();

		void* tempKey = _delegateKeys[len - 1];
		_delegateKeys[len - 1] = _delegateKeys[index];
		_delegateKeys[index] = tempKey;
		_delegateKeys.pop_back();
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


#define JG_DECLARE_MULTI_DELEGATE(DelegateName) \
using DelegateName = PMultiDelegate<>;\

#define JG_DECLARE_MULTI_DELEGATE_ONEPARAM(DelegateName, OneParam)\
using DelegateName   = PMultiDelegate<##OneParam>;\

#define JG_DECLARE_MULTI_DELEGATE_TWOPARAM(DelegateName, OneParam, TwoParam) \
using DelegateName   = PMultiDelegate<##OneParam, ##TwoParam>;\

#define JG_DECLARE_MULTI_DELEGATE_THREEPARAM(DelegateName, OneParam, TwoParam, ThreeParam) \
using DelegateName   = PMultiDelegate<##OneParam, ##TwoParam, ##ThreeParam>; \

#define JG_DECLARE_MULTI_DELEGATE_FOURPARAM(DelegateName, OneParam, TwoParam, ThreeParam, FourParam) \
using DelegateName   = PMultiDelegate<##OneParam, ##TwoParam, ##ThreeParam, ##FourParam>; \

#define JG_DECLARE_MULTI_DELEGATE_FIVEPARAM(DelegateName, OneParam, TwoParam, ThreeParam, FourParam, FiveParam) \
using DelegateName   = PMultiDelegate<##OneParam, ##TwoParam, ##ThreeParam, ##FourParam, ##FiveParam>; \

#define JG_DELEGATE_FN_BIND(fn) std::bind(&##fn, this)
#define JG_DELEGATE_FN_BIND_ONEPARAM(fn) std::bind(&##fn, this, std::placeholders::_1)
#define JG_DELEGATE_FN_BIND_TWOPARAM(fn) std::bind(&##fn, this, std::placeholders::_1, std::placeholders::_2)
#define JG_DELEGATE_FN_BIND_THREEPARAM(fn) std::bind(&##fn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define JG_DELEGATE_FN_BIND_FOURPARAM(fn) std::bind(&##fn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
#define JG_DELEGATE_FN_BIND_FIVEPARAM(fn) std::bind(&##fn, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)



