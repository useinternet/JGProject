#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "Platform/Platform.h"
#include "Misc/Guid.h"
#include "Object/JGType.h"
#include "Object/JGObject.h"
#include <functional>

#ifndef DELEGATE_ASSERT
#include <assert.h>
#define DELEGATE_ASSERT(expression, ...) assert(expression)
#endif

#ifndef DELEGATE_STATIC_ASSERT
#if __cplusplus <= 199711L
#define DELEGATE_STATIC_ASSERT(expression, msg) static_assert(expression, msg)
#else
#define DELEGATE_STATIC_ASSERT(expression, msg)
#endif
#endif

//The allocation size of delegate data.
//Delegates larger than this will be heap allocated.
#ifndef DELEGATE_INLINE_ALLOCATION_SIZE
#define DELEGATE_INLINE_ALLOCATION_SIZE 32
#endif

#define JG_DECLARE_DELEGATE(Name, ...) \
using Name = HDelegate<void, __VA_ARGS__>;

#define JG_DECLARE_DELEGATE_RET(Name, retValue, ...) \
using Name = HDelegate<retValue, __VA_ARGS__>;

#define JG_DECLARE_MULTICAST_DELEGATE(Name, ...) \
using Name = HMulticastDelegate<__VA_ARGS__>; \
using Name ## Delegate = HMulticastDelegate<__VA_ARGS__>::DelegateT; \

#define JG_DECLARE_EVENT(Name, OwnerType, ...) \
class Name : public HMulticastDelegate<__VA_ARGS__>; \
{ \
private: \
	friend class OwnerType; \
	using HMulticastDelegate::Broadcast; \
	using HMulticastDelegate::RemoveAll; \
	using HMulticastDelegate::Remove; \
}; \

namespace HDelegatesInteral
{
	template<bool IsConst, typename Object, typename RetVal, typename ...Args>
	struct HMemberFunction;

	template<typename Object, typename RetVal, typename ...Args>
	struct HMemberFunction<true, Object, RetVal, Args...>
	{
		using Type = RetVal(Object::*)(Args...) const;
	};

	template<typename Object, typename RetVal, typename ...Args>
	struct HMemberFunction<false, Object, RetVal, Args...>
	{
		using Type = RetVal(Object::*)(Args...);
	};

	static void* (*Alloc)(size_t size) = [](size_t size) { return malloc(size); };
	static void(*Free)(void* pPtr) = [](void* pPtr) { free(pPtr); };

	template<typename T>
	void DelegateDeleteFunc(T* pPtr)
	{
		pPtr->~T();
		DelegateFreeFunc(pPtr);
	}
}

namespace HDelegates
{
	using AllocateCallback = void* (*)(size_t size);
	using FreeCallback = void(*)(void* pPtr);
	inline void SetAllocationCallbacks(AllocateCallback allocateCallback, FreeCallback freeCallback)
	{
		HDelegatesInteral::Alloc = allocateCallback;
		HDelegatesInteral::Free  = freeCallback;
	}
}

class IDelegateInstanceBase : public IMemoryObject
{
public:
	IDelegateInstanceBase() = default;
	virtual ~IDelegateInstanceBase() noexcept = default;
	virtual const void* GetOwner() const { return nullptr; }
	virtual void Clone(void* pDestination) = 0;

	virtual JGType GetType() const = 0;
};

//Base type for delegates
template<typename RetVal, typename... Args>
class IDelegateInstance : public IDelegateInstanceBase
{
public:
	virtual RetVal Execute(Args&&... args) = 0;
	virtual JGType GetType() const { return JGType::GenerateType<IDelegateInstance<RetVal, Args...>>();  }
};

template<typename RetVal, typename... Args2>
class PStaticDelegate;

template<typename RetVal, typename... Args, typename... Args2>
class PStaticDelegate<RetVal(Args...), Args2...> : public IDelegateInstance<RetVal, Args...>
{
public:
	using DelegateFunction = RetVal(*)(Args..., Args2...);

	PStaticDelegate(DelegateFunction function, Args2&&... payload)
		: _function(function), _payload(std::forward<Args2>(payload)...)
	{}

	PStaticDelegate(DelegateFunction function, const std::tuple<Args2...>& payload)
		: _function(function), _payload(payload)
	{}

	virtual RetVal Execute(Args&&... args) override
	{
		return Execute_Internal(std::forward<Args>(args)..., std::index_sequence_for<Args2...>());
	}

	virtual void Clone(void* pDestination) override
	{
		new (pDestination) PStaticDelegate(_function, _payload);
	}

private:
	template<std::size_t... Is>
	RetVal Execute_Internal(Args&&... args, std::index_sequence<Is...>)
	{
		return _function(std::forward<Args>(args)..., std::get<Is>(_payload)...);
	}

	DelegateFunction _function;
	std::tuple<Args2...> _payload;
};

template<bool IsConst, typename T, typename RetVal, typename... Args2>
class PRawDelegate;

template<bool IsConst, typename T, typename RetVal, typename... Args, typename... Args2>
class PRawDelegate<IsConst, T, RetVal(Args...), Args2...> : public IDelegateInstance<RetVal, Args...>
{
public:
	using DelegateFunction = typename HDelegatesInteral::HMemberFunction<IsConst, T, RetVal, Args..., Args2...>::Type;

	PRawDelegate(T* pObject, DelegateFunction function, Args2&&... payload)
		: _pObject(pObject), _function(function), _payload(std::forward<Args2>(payload)...)
	{}

	PRawDelegate(T* pObject, DelegateFunction function, const std::tuple<Args2...>& payload)
		: _pObject(pObject), _function(function), _payload(payload)
	{}

	virtual RetVal Execute(Args&&... args) override
	{
		return Execute_Internal(std::forward<Args>(args)..., std::index_sequence_for<Args2...>());
	}
	virtual const void* GetOwner() const override
	{
		return _pObject;
	}

	virtual void Clone(void* pDestination) override
	{
		new (pDestination) PRawDelegate(_pObject, _function, _payload);
	}

private:
	template<std::size_t... Is>
	RetVal Execute_Internal(Args&&... args, std::index_sequence<Is...>)
	{
		return (_pObject->*_function)(std::forward<Args>(args)..., std::get<Is>(_payload)...);
	}

	T* _pObject;
	DelegateFunction _function;
	std::tuple<Args2...> _payload;
};

template<typename TLambda, typename RetVal, typename... Args>
class PLambdaDelegate;

template<typename TLambda, typename RetVal, typename... Args, typename... Args2>
class PLambdaDelegate<TLambda, RetVal(Args...), Args2...> : public IDelegateInstance<RetVal, Args...>
{
public:
	explicit PLambdaDelegate(TLambda&& lambda, Args2&&... payload)
		: _lambda(std::forward<TLambda>(lambda)),
		_payload(std::forward<Args2>(payload)...)
	{}

	explicit PLambdaDelegate(const TLambda& lambda, const std::tuple<Args2...>& payload)
		: _lambda(lambda),
		_payload(payload)
	{}

	RetVal Execute(Args&&... args) override
	{
		return Execute_Internal(std::forward<Args>(args)..., std::index_sequence_for<Args2...>());
	}

	virtual void Clone(void* pDestination) override
	{
		new (pDestination) PLambdaDelegate(_lambda, _payload);
	}

private:
	template<std::size_t... Is>
	RetVal Execute_Internal(Args&&... args, std::index_sequence<Is...>)
	{
		return (RetVal)((_lambda)(std::forward<Args>(args)..., std::get<Is>(_payload)...));
	}

	TLambda _lambda;
	std::tuple<Args2...> _payload;
};

template<bool IsConst, typename T, typename RetVal, typename... Args>
class PSPDelegate;

template<bool IsConst, typename RetVal, typename T, typename... Args, typename... Args2>
class PSPDelegate<IsConst, T, RetVal(Args...), Args2...> : public IDelegateInstance<RetVal, Args...>
{
public:
	using DelegateFunction = typename HDelegatesInteral::HMemberFunction<IsConst, T, RetVal, Args..., Args2...>::Type;

	PSPDelegate(PSharedPtr<T> pObject, DelegateFunction pFunction, Args2&&... payload)
		: _object(pObject),
		_function(pFunction),
		_payload(std::forward<Args2>(payload)...)
	{}

	PSPDelegate(PWeakPtr<T> pObject, DelegateFunction pFunction, const std::tuple<Args2...>& payload)
		: _object(pObject),
		_function(pFunction),
		_payload(payload)
	{}

	virtual RetVal Execute(Args&&... args) override
	{
		return Execute_Internal(std::forward<Args>(args)..., std::index_sequence_for<Args2...>());
	}

	virtual const void* GetOwner() const override
	{
		return _object.IsValid() ? nullptr : _object.Pin().GetRawConstPointer();
	}

	virtual void Clone(void* pDestination) override
	{
		new (pDestination) PSPDelegate(_object, _function, _payload);
	}

private:
	template<std::size_t... Is>
	RetVal Execute_Internal(Args&&... args, std::index_sequence<Is...>)
	{
		if (_object.IsValid() == false)
		{
			return RetVal();
		}
		else
		{
			PSharedPtr<T> pPinned = _object.Pin();
			return (pPinned.GetRawPointer()->*_function)(std::forward<Args>(args)..., std::get<Is>(_payload)...);
		}
	}

	PWeakPtr<T> _object;
	DelegateFunction _function;
	std::tuple<Args2...> _payload;
};

class HDelegateHandle
{
public:
	constexpr HDelegateHandle() noexcept
		: _id(INVALID_ID)
	{
	}

	explicit HDelegateHandle(bool /*generateId*/) noexcept
		: _id(GetNewID())
	{
	}

	~HDelegateHandle() noexcept = default;
	HDelegateHandle(const HDelegateHandle& other) = default;
	HDelegateHandle& operator=(const HDelegateHandle& other) = default;

	HDelegateHandle(HDelegateHandle&& other) noexcept
		: _id(other._id)
	{
		other.Reset();
	}

	HDelegateHandle& operator=(HDelegateHandle&& other) noexcept
	{
		_id = other._id;
		other.Reset();
		return *this;
	}

	operator bool() const noexcept
	{
		return IsValid();
	}

	bool operator==(const HDelegateHandle& other) const noexcept
	{
		return _id == other._id;
	}

	bool operator<(const HDelegateHandle& other) const noexcept
	{
		return _id < other._id;
	}

	bool IsValid() const noexcept
	{
		return _id != INVALID_ID;
	}

	void Reset() noexcept
	{
		_id = INVALID_ID;
	}

	constexpr static const uint64 INVALID_ID = (uint64)~0;
private:
	uint64 _id;
	static uint64 GetNewID()
	{
		return HPlatform::NewGuid().GetHashCode();
	}
};

template<size_t MaxStackSize>
class HInlineAllocator
{
public:
	constexpr HInlineAllocator() noexcept
		: _size(0)
	{
		DELEGATE_STATIC_ASSERT(MaxStackSize > sizeof(void*), "MaxStackSize is smaller or equal to the size of a pointer. This will make the use of an InlineAllocator pointless. Please increase the MaxStackSize.");
	}

	~HInlineAllocator() noexcept
	{
		Free();
	}

	//Copy constructor
	HInlineAllocator(const HInlineAllocator& other)
		: _size(0)
	{
		if (other.HasAllocation())
		{
			memcpy(Allocate(other._size), other.GetAllocation(), other._size);
		}
		_size = other._size;
	}

	//Copy assignment operator
	HInlineAllocator& operator=(const HInlineAllocator& other)
	{
		if (other.HasAllocation())
		{
			memcpy(Allocate(other._size), other.GetAllocation(), other._size);
		}
		_size = other._size;
		return *this;
	}

	//Move constructor
	HInlineAllocator(HInlineAllocator&& other) noexcept
		: _size(other._size)
	{
		other._size = 0;
		if (_size > MaxStackSize)
		{
			std::swap(pPtr, other.pPtr);
		}
		else
		{
			memcpy(Buffer, other.Buffer, _size);
		}
	}

	//Move assignment operator
	HInlineAllocator& operator=(HInlineAllocator&& other) noexcept
	{
		Free();
		_size = other._size;
		other._size = 0;
		if (_size > MaxStackSize)
		{
			std::swap(pPtr, other.pPtr);
		}
		else
		{
			memcpy(Buffer, other.Buffer, _size);
		}
		return *this;
	}

	void* Allocate(const size_t size)
	{
		if (_size != size)
		{
			Free();
			_size = size;
			if (size > MaxStackSize)
			{
				pPtr = HDelegatesInteral::Alloc(size);
				return pPtr;
			}
		}
		return (void*)Buffer;
	}

	//Free the allocated memory
	void Free()
	{
		if (_size > MaxStackSize)
		{
			HDelegatesInteral::Free(pPtr);
		}
		_size = 0;
	}

	//Return the allocated memory either on the stack or on the heap
	void* GetAllocation() const
	{
		if (HasAllocation())
		{
			return HasHeapAllocation() ? pPtr : (void*)Buffer;
		}
		else
		{
			return nullptr;
		}
	}

	uint64 GetSize() const
	{
		return _size;
	}

	bool HasAllocation() const
	{
		return _size > 0;
	}

	bool HasHeapAllocation() const
	{
		return _size > MaxStackSize;
	}

private:
	union
	{
		char  Buffer[MaxStackSize];
		void* pPtr;
	};
	uint64 _size;
};

class HDelegateBase
{
public:
	constexpr HDelegateBase() noexcept
		: _allocator()
	{}

	virtual ~HDelegateBase() noexcept
	{
		Release();
	}

	HDelegateBase(const HDelegateBase& other)
	{
		if (other._allocator.HasAllocation())
		{
			_allocator.Allocate(other._allocator.GetSize());
			other.GetDelegate()->Clone(_allocator.GetAllocation());
		}
	}

	HDelegateBase& operator=(const HDelegateBase& other)
	{
		Release();
		if (other._allocator.HasAllocation())
		{
			_allocator.Allocate(other._allocator.GetSize());
			other.GetDelegate()->Clone(_allocator.GetAllocation());
		}
		return *this;
	}

	HDelegateBase(HDelegateBase&& other) noexcept
		: _allocator(std::move(other._allocator))
	{}

	HDelegateBase& operator=(HDelegateBase&& other) noexcept
	{
		Release();
		_allocator = std::move(other._allocator);
		return *this;
	}

	const void* GetOwner() const
	{
		if (_allocator.HasAllocation())
		{
			return GetDelegate()->GetOwner();
		}
		return nullptr;
	}

	uint64 GetSize() const
	{
		return _allocator.GetSize();
	}

	void ClearIfBoundTo(void* pObject)
	{
		if (pObject != nullptr && IsBoundTo(pObject))
		{
			Release();
		}
	}

	void Clear()
	{
		Release();
	}

	bool IsBound() const
	{
		return _allocator.HasAllocation();
	}

	bool IsBoundTo(void* pObject) const
	{
		if (pObject == nullptr || _allocator.HasAllocation() == false)
		{
			return false;
		}
		return GetDelegate()->GetOwner() == pObject;
	}

protected:
	void Release()
	{
		if (_allocator.HasAllocation())
		{
			GetDelegate()->~IDelegateInstanceBase();
			_allocator.Free();
		}
	}

	IDelegateInstanceBase* GetDelegate() const
	{
		return static_cast<IDelegateInstanceBase*>(_allocator.GetAllocation());
	}

	HInlineAllocator<DELEGATE_INLINE_ALLOCATION_SIZE> _allocator;
};

template<typename RetVal, typename... Args>
class HDelegate : public HDelegateBase
{
private:
	template<typename T, typename... Args2>
	using ConstMemberFunction = typename HDelegatesInteral::HMemberFunction<true, T, RetVal, Args..., Args2...>::Type;
	template<typename T, typename... Args2>
	using NonConstMemberFunction = typename HDelegatesInteral::HMemberFunction<false, T, RetVal, Args..., Args2...>::Type;

public:
	using IDelegateT = IDelegateInstance<RetVal, Args...>;

	template<typename T, typename... Args2>
	[[nodiscard]] static HDelegate CreateRaw(T* pObj, NonConstMemberFunction<T, Args2...> pFunction, Args2... args)
	{
		HDelegate handler;
		handler.Bind<PRawDelegate<false, T, RetVal(Args...), Args2...>>(pObj, pFunction, std::forward<Args2>(args)...);
		return handler;
	}

	template<typename T, typename... Args2>
	[[nodiscard]] static HDelegate CreateRaw(T* pObj, ConstMemberFunction<T, Args2...> pFunction, Args2... args)
	{
		HDelegate handler;
		handler.Bind<PRawDelegate<true, T, RetVal(Args...), Args2...>>(pObj, pFunction, std::forward<Args2>(args)...);
		return handler;
	}

	template<typename... Args2>
	[[nodiscard]] static HDelegate CreateStatic(RetVal(*pFunction)(Args..., Args2...), Args2... args)
	{
		HDelegate handler;
		handler.Bind<PStaticDelegate<RetVal(Args...), Args2...>>(pFunction, std::forward<Args2>(args)...);
		return handler;
	}

	template<typename T, typename... Args2>
	[[nodiscard]] static HDelegate CreateSP(const PSharedPtr<T>& pObject, NonConstMemberFunction<T, Args2...> pFunction, Args2... args)
	{
		HDelegate handler;
		handler.Bind<PSPDelegate<false, T, RetVal(Args...), Args2...>>(pObject, pFunction, std::forward<Args2>(args)...);
		return handler;
	}

	template<typename T, typename... Args2>
	[[nodiscard]] static HDelegate CreateSP(const PSharedPtr<T>& pObject, ConstMemberFunction<T, Args2...> pFunction, Args2... args)
	{
		HDelegate handler;
		handler.Bind<PSPDelegate<true, T, RetVal(Args...), Args2...>>(pObject, pFunction, std::forward<Args2>(args)...);
		return handler;
	}

	template<typename TLambda, typename... Args2>
	[[nodiscard]] static HDelegate CreateLambda(TLambda&& lambda, Args2... args)
	{
		HDelegate handler;
		handler.Bind<PLambdaDelegate<TLambda, RetVal(Args...), Args2...>>(std::forward<TLambda>(lambda), std::forward<Args2>(args)...);
		return handler;
	}

	template<typename T, typename... Args2>
	void BindRaw(T* pObject, NonConstMemberFunction<T, Args2...> pFunction, Args2&&... args)
	{
		DELEGATE_STATIC_ASSERT(!std::is_const<T>::value, "Cannot bind a non-const function on a const object");
		*this = CreateRaw<T, Args2... >(pObject, pFunction, std::forward<Args2>(args)...);
	}

	template<typename T, typename... Args2>
	void BindRaw(T* pObject, ConstMemberFunction<T, Args2...> pFunction, Args2&&... args)
	{
		*this = CreateRaw<T, Args2... >(pObject, pFunction, std::forward<Args2>(args)...);
	}

	template<typename... Args2>
	void BindStatic(RetVal(*pFunction)(Args..., Args2...), Args2&&... args)
	{
		*this = CreateStatic<Args2... >(pFunction, std::forward<Args2>(args)...);
	}

	template<typename LambdaType, typename... Args2>
	void BindLambda(LambdaType&& lambda, Args2&&... args)
	{
		*this = CreateLambda<LambdaType, Args2... >(std::forward<LambdaType>(lambda), std::forward<Args2>(args)...);
	}

	template<typename T, typename... Args2>
	void BindSP(std::shared_ptr<T> pObject, NonConstMemberFunction<T, Args2...> pFunction, Args2&&... args)
	{
		DELEGATE_STATIC_ASSERT(!std::is_const<T>::value, "Cannot bind a non-const function on a const object");
		*this = CreateSP<T, Args2... >(pObject, pFunction, std::forward<Args2>(args)...);
	}

	template<typename T, typename... Args2>
	void BindSP(std::shared_ptr<T> pObject, ConstMemberFunction<T, Args2...> pFunction, Args2&&... args)
	{
		*this = CreateSP<T, Args2... >(pObject, pFunction, std::forward<Args2>(args)...);
	}

	RetVal Execute(Args... args) const
	{
		DELEGATE_ASSERT(_allocator.HasAllocation(), "Delegate is not bound");
		return ((IDelegateT*)GetDelegate())->Execute(std::forward<Args>(args)...);
	}

	RetVal ExecuteIfBound(Args... args) const
	{
		if (IsBound())
		{
			return ((IDelegateT*)GetDelegate())->Execute(std::forward<Args>(args)...);
		}
		return RetVal();
	}

private:
	template<typename T, typename... Args3>
	void Bind(Args3&&... args)
	{
		Release();
		void* pAlloc = _allocator.Allocate(sizeof(T));
		new (pAlloc) T(std::forward<Args3>(args)...);
	}
};

template<typename... Args>
class HMulticastDelegate : public HDelegateBase
{
public:
	using DelegateT = HDelegate<void, Args...>;

private:
	struct DelegateHandlerPair
	{
		HDelegateHandle Handle;
		DelegateT Callback;
		DelegateHandlerPair() : Handle(false) {}
		DelegateHandlerPair(const HDelegateHandle& handle, const DelegateT& callback) : Handle(handle), Callback(callback) {}
		DelegateHandlerPair(const HDelegateHandle& handle, DelegateT&& callback) : Handle(handle), Callback(std::move(callback)) {}
	};
	template<typename T, typename... Args2>
	using ConstMemberFunction = typename HDelegatesInteral::HMemberFunction<true, T, void, Args..., Args2...>::Type;
	template<typename T, typename... Args2>
	using NonConstMemberFunction = typename HDelegatesInteral::HMemberFunction<false, T, void, Args..., Args2...>::Type;

public:
	constexpr HMulticastDelegate()
		: _locks(0)
	{
	}

	~HMulticastDelegate() noexcept = default;
	HMulticastDelegate(const HMulticastDelegate& other) = default;
	HMulticastDelegate& operator=(const HMulticastDelegate& other) = default;
	HMulticastDelegate(HMulticastDelegate&& other) noexcept
		: _events(std::move(other._events)),
		_locks(std::move(other._locks))
	{
	}
	HMulticastDelegate& operator=(HMulticastDelegate&& other) noexcept
	{
		_events = std::move(other._events);
		_locks = std::move(other._locks);
		return *this;
	}

	template<typename T>
	HDelegateHandle operator+=(T&& l)
	{
		return Add(DelegateT::CreateLambda(std::move(l)));
	}

	HDelegateHandle operator+=(DelegateT&& handler) noexcept
	{
		return Add(std::forward<DelegateT>(handler));
	}

	bool operator-=(HDelegateHandle& handle)
	{
		return Remove(handle);
	}

	HDelegateHandle Add(DelegateT&& handler) noexcept
	{
		for (size_t i = 0; i < _events.size(); ++i)
		{
			if (_events[i].Handle.IsValid() == false)
			{
				_events[i] = DelegateHandlerPair(HDelegateHandle(true), std::move(handler));
				return _events[i].Handle;
			}
		}
		_events.emplace_back(HDelegateHandle(true), std::move(handler));
		return _events.back().Handle;
	}

	template<typename T, typename... Args2>
	HDelegateHandle AddRaw(T* pObject, NonConstMemberFunction<T, Args2...> pFunction, Args2&&... args)
	{
		return Add(DelegateT::CreateRaw(pObject, pFunction, std::forward<Args2>(args)...));
	}

	template<typename T, typename... Args2>
	HDelegateHandle AddRaw(T* pObject, ConstMemberFunction<T, Args2...> pFunction, Args2&&... args)
	{
		return Add(DelegateT::CreateRaw(pObject, pFunction, std::forward<Args2>(args)...));
	}

	template<typename... Args2>
	HDelegateHandle AddStatic(void(*pFunction)(Args..., Args2...), Args2&&... args)
	{
		return Add(DelegateT::CreateStatic(pFunction, std::forward<Args2>(args)...));
	}

	template<typename LambdaType, typename... Args2>
	HDelegateHandle AddLambda(LambdaType&& lambda, Args2&&... args)
	{
		return Add(DelegateT::CreateLambda(std::forward<LambdaType>(lambda), std::forward<Args2>(args)...));
	}

	template<typename T, typename... Args2>
	HDelegateHandle AddSP(PSharedPtr<T> pObject, NonConstMemberFunction<T, Args2...> pFunction, Args2&&... args)
	{
		return Add(DelegateT::CreateSP(pObject, pFunction, std::forward<Args2>(args)...));
	}

	template<typename T, typename... Args2>
	HDelegateHandle AddSP(PSharedPtr<T> pObject, ConstMemberFunction<T, Args2...> pFunction, Args2&&... args)
	{
		return Add(DelegateT::CreateSP(pObject, pFunction, std::forward<Args2>(args)...));
	}

	void RemoveObject(void* pObject)
	{
		if (pObject != nullptr)
		{
			for (uint64 i = 0; i < _events.size(); ++i)
			{
				if (_events[i].Callback.GetOwner() == pObject)
				{
					if (IsLocked())
					{
						_events[i].Callback.Clear();
					}
					else
					{
						std::swap(_events[i], _events[_events.size() - 1]);
						_events.pop_back();
					}
				}
			}
		}
	}

	bool Remove(HDelegateHandle& handle)
	{
		if (handle.IsValid())
		{
			for (uint64 i = 0; i < _events.size(); ++i)
			{
				if (_events[i].Handle == handle)
				{
					if (IsLocked())
					{
						_events[i].Callback.Clear();
					}
					else
					{
						std::swap(_events[i], _events[_events.size() - 1]);
						_events.pop_back();
					}
					handle.Reset();
					return true;
				}
			}
		}
		return false;
	}

	bool IsBoundTo(const HDelegateHandle& handle) const
	{
		if (handle.IsValid())
		{
			for (uint64 i = 0; i < _events.size(); ++i)
			{
				if (_events[i].Handle == handle)
				{
					return true;
				}
			}
		}
		return false;
	}

	void RemoveAll()
	{
		if (IsLocked())
		{
			for (DelegateHandlerPair& handler : _events)
			{
				handler.Callback.Clear();
			}
		}
		else
		{
			_events.clear();
		}
	}

	void Compress(const uint64 maxSpace = 0)
	{
		if (IsLocked() == false)
		{
			uint64 toDelete = 0;
			for (uint64 i = 0; i < _events.size() - toDelete; ++i)
			{
				if (_events[i].Handle.IsValid() == false)
				{
					std::swap(_events[i], _events[toDelete]);
					++toDelete;
				}
			}
			if (toDelete > maxSpace)
			{
				_events.resize(_events.size() - toDelete);
			}
		}
	}

	void BroadCast(Args ...args)
	{
		Lock();
		for (size_t i = 0; i < _events.size(); ++i)
		{
			if (_events[i].Handle.IsValid())
			{
				_events[i].Callback.Execute(std::forward<Args>(args)...);
			}
		}
		Unlock();
	}

	size_t GetSize() const
	{
		return _events.size();
	}

private:
	void Lock()
	{
		++_locks;
	}

	void Unlock()
	{
		DELEGATE_ASSERT(_locks > 0);
		--_locks;
	}

	bool IsLocked() const
	{
		return _locks > 0;
	}

	HList<DelegateHandlerPair> _events;
	uint32 _locks;
};