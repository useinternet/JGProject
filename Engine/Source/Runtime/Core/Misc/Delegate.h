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

#define JG_DECLARE_DELEGATE(name, ...) \
using name = HDelegate<void, __VA_ARGS__>;

#define JG_DECLARE_DELEGATE_RET(name, retValue, ...) \
using name = HDelegate<retValue, __VA_ARGS__>;

#define JG_DECLARE_MULTICAST_DELEGATE(name, ...) \
using name = HMulticastDelegate<__VA_ARGS__>; \
using name ## Delegate = HMulticastDelegate<__VA_ARGS__>::DelegateT; \

#define JG_DECLARE_EVENT(name, ownerType, ...) \
class name : public HMulticastDelegate<__VA_ARGS__>; \
{ \
private: \
	friend class ownerType; \
	using HMulticastDelegate::Broadcast; \
	using HMulticastDelegate::RemoveAll; \
	using HMulticastDelegate::Remove; \
}; \

///////////////////////////////////////////////////////////////
/////////////////// INTERNAL SECTION //////////////////////////
///////////////////////////////////////////////////////////////

#if __cplusplus >= 201703L
#define NO_DISCARD [[nodiscard]]
#else
#define NO_DISCARD		
#endif

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
		HDelegatesInteral::Free = freeCallback;
	}
}
//IDelegateInstance
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
		: m_pObject(pObject), m_Function(function), m_Payload(std::forward<Args2>(payload)...)
	{}

	PRawDelegate(T* pObject, DelegateFunction function, const std::tuple<Args2...>& payload)
		: m_pObject(pObject), m_Function(function), m_Payload(payload)
	{}

	virtual RetVal Execute(Args&&... args) override
	{
		return Execute_Internal(std::forward<Args>(args)..., std::index_sequence_for<Args2...>());
	}
	virtual const void* GetOwner() const override
	{
		return m_pObject;
	}

	virtual void Clone(void* pDestination) override
	{
		new (pDestination) PRawDelegate(m_pObject, m_Function, m_Payload);
	}

private:
	template<std::size_t... Is>
	RetVal Execute_Internal(Args&&... args, std::index_sequence<Is...>)
	{
		return (m_pObject->*m_Function)(std::forward<Args>(args)..., std::get<Is>(m_Payload)...);
	}

	T* m_pObject;
	DelegateFunction m_Function;
	std::tuple<Args2...> m_Payload;
};

template<typename TLambda, typename RetVal, typename... Args>
class PLambdaDelegate;

template<typename TLambda, typename RetVal, typename... Args, typename... Args2>
class PLambdaDelegate<TLambda, RetVal(Args...), Args2...> : public IDelegateInstance<RetVal, Args...>
{
public:
	explicit PLambdaDelegate(TLambda&& lambda, Args2&&... payload)
		: m_Lambda(std::forward<TLambda>(lambda)),
		m_Payload(std::forward<Args2>(payload)...)
	{}

	explicit PLambdaDelegate(const TLambda& lambda, const std::tuple<Args2...>& payload)
		: m_Lambda(lambda),
		m_Payload(payload)
	{}

	RetVal Execute(Args&&... args) override
	{
		return Execute_Internal(std::forward<Args>(args)..., std::index_sequence_for<Args2...>());
	}

	virtual void Clone(void* pDestination) override
	{
		new (pDestination) PLambdaDelegate(m_Lambda, m_Payload);
	}

private:
	template<std::size_t... Is>
	RetVal Execute_Internal(Args&&... args, std::index_sequence<Is...>)
	{
		return (RetVal)((m_Lambda)(std::forward<Args>(args)..., std::get<Is>(m_Payload)...));
	}

	TLambda m_Lambda;
	std::tuple<Args2...> m_Payload;
};

template<bool IsConst, typename T, typename RetVal, typename... Args>
class PSPDelegate;

template<bool IsConst, typename RetVal, typename T, typename... Args, typename... Args2>
class PSPDelegate<IsConst, T, RetVal(Args...), Args2...> : public IDelegateInstance<RetVal, Args...>
{
public:
	using DelegateFunction = typename HDelegatesInteral::HMemberFunction<IsConst, T, RetVal, Args..., Args2...>::Type;

	PSPDelegate(PSharedPtr<T> pObject, DelegateFunction pFunction, Args2&&... payload)
		: m_pObject(pObject),
		m_pFunction(pFunction),
		m_Payload(std::forward<Args2>(payload)...)
	{}

	PSPDelegate(PWeakPtr<T> pObject, DelegateFunction pFunction, const std::tuple<Args2...>& payload)
		: m_pObject(pObject),
		m_pFunction(pFunction),
		m_Payload(payload)
	{}

	virtual RetVal Execute(Args&&... args) override
	{
		return Execute_Internal(std::forward<Args>(args)..., std::index_sequence_for<Args2...>());
	}

	virtual const void* GetOwner() const override
	{
		return m_pObject.IsValid() ? nullptr : m_pObject.Pin().GetRawConstPointer();
	}

	virtual void Clone(void* pDestination) override
	{
		new (pDestination) PSPDelegate(m_pObject, m_pFunction, m_Payload);
	}

private:
	template<std::size_t... Is>
	RetVal Execute_Internal(Args&&... args, std::index_sequence<Is...>)
	{
		if (m_pObject.IsValid() == false)
		{
			return RetVal();
		}
		else
		{
			PSharedPtr<T> pPinned = m_pObject.Pin();
			return (pPinned.GetRawPointer()->*m_pFunction)(std::forward<Args>(args)..., std::get<Is>(m_Payload)...);
		}
	}

	PWeakPtr<T> m_pObject;
	DelegateFunction m_pFunction;
	std::tuple<Args2...> m_Payload;
};

//A handle to a delegate used for a multicast delegate
//Static ID so that every handle is unique
class HDelegateHandle
{
public:
	constexpr HDelegateHandle() noexcept
		: m_Id(INVALID_ID)
	{
	}

	explicit HDelegateHandle(bool /*generateId*/) noexcept
		: m_Id(GetNewID())
	{
	}

	~HDelegateHandle() noexcept = default;
	HDelegateHandle(const HDelegateHandle& other) = default;
	HDelegateHandle& operator=(const HDelegateHandle& other) = default;

	HDelegateHandle(HDelegateHandle&& other) noexcept
		: m_Id(other.m_Id)
	{
		other.Reset();
	}

	HDelegateHandle& operator=(HDelegateHandle&& other) noexcept
	{
		m_Id = other.m_Id;
		other.Reset();
		return *this;
	}

	operator bool() const noexcept
	{
		return IsValid();
	}

	bool operator==(const HDelegateHandle& other) const noexcept
	{
		return m_Id == other.m_Id;
	}

	bool operator<(const HDelegateHandle& other) const noexcept
	{
		return m_Id < other.m_Id;
	}

	bool IsValid() const noexcept
	{
		return m_Id != INVALID_ID;
	}

	void Reset() noexcept
	{
		m_Id = INVALID_ID;
	}

	constexpr static const uint64 INVALID_ID = (uint64)~0;
private:
	uint64 m_Id;
	static uint64 GetNewID()
	{
		return HPlatform::NewGuid().GetHashCode();
	}
};

template<size_t MaxStackSize>
class HInlineAllocator
{
public:
	//Constructor
	constexpr HInlineAllocator() noexcept
		: m_Size(0)
	{
		DELEGATE_STATIC_ASSERT(MaxStackSize > sizeof(void*), "MaxStackSize is smaller or equal to the size of a pointer. This will make the use of an InlineAllocator pointless. Please increase the MaxStackSize.");
	}

	//Destructor
	~HInlineAllocator() noexcept
	{
		Free();
	}

	//Copy constructor
	HInlineAllocator(const HInlineAllocator& other)
		: m_Size(0)
	{
		if (other.HasAllocation())
		{
			memcpy(Allocate(other.m_Size), other.GetAllocation(), other.m_Size);
		}
		m_Size = other.m_Size;
	}

	//Copy assignment operator
	HInlineAllocator& operator=(const HInlineAllocator& other)
	{
		if (other.HasAllocation())
		{
			memcpy(Allocate(other.m_Size), other.GetAllocation(), other.m_Size);
		}
		m_Size = other.m_Size;
		return *this;
	}

	//Move constructor
	HInlineAllocator(HInlineAllocator&& other) noexcept
		: m_Size(other.m_Size)
	{
		other.m_Size = 0;
		if (m_Size > MaxStackSize)
		{
			std::swap(pPtr, other.pPtr);
		}
		else
		{
			memcpy(Buffer, other.Buffer, m_Size);
		}
	}

	//Move assignment operator
	HInlineAllocator& operator=(HInlineAllocator&& other) noexcept
	{
		Free();
		m_Size = other.m_Size;
		other.m_Size = 0;
		if (m_Size > MaxStackSize)
		{
			std::swap(pPtr, other.pPtr);
		}
		else
		{
			memcpy(Buffer, other.Buffer, m_Size);
		}
		return *this;
	}

	//Allocate memory of given size
	//If the size is over the predefined threshold, it will be allocated on the heap
	void* Allocate(const size_t size)
	{
		if (m_Size != size)
		{
			Free();
			m_Size = size;
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
		if (m_Size > MaxStackSize)
		{
			HDelegatesInteral::Free(pPtr);
		}
		m_Size = 0;
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

	size_t GetSize() const
	{
		return m_Size;
	}

	bool HasAllocation() const
	{
		return m_Size > 0;
	}

	bool HasHeapAllocation() const
	{
		return m_Size > MaxStackSize;
	}

private:
	//If the allocation is smaller than the threshold, Buffer is used
	//Otherwise pPtr is used together with a separate dynamic allocation
	union
	{
		char Buffer[MaxStackSize];
		void* pPtr;
	};
	size_t m_Size;
};

class HDelegateBase
{
public:
	//Default constructor
	constexpr HDelegateBase() noexcept
		: m_Allocator()
	{}

	//Default destructor
	virtual ~HDelegateBase() noexcept
	{
		Release();
	}

	//Copy contructor
	HDelegateBase(const HDelegateBase& other)
	{
		if (other.m_Allocator.HasAllocation())
		{
			m_Allocator.Allocate(other.m_Allocator.GetSize());
			other.GetDelegate()->Clone(m_Allocator.GetAllocation());
		}
	}

	//Copy assignment operator
	HDelegateBase& operator=(const HDelegateBase& other)
	{
		Release();
		if (other.m_Allocator.HasAllocation())
		{
			m_Allocator.Allocate(other.m_Allocator.GetSize());
			other.GetDelegate()->Clone(m_Allocator.GetAllocation());
		}
		return *this;
	}

	//Move constructor
	HDelegateBase(HDelegateBase&& other) noexcept
		: m_Allocator(std::move(other.m_Allocator))
	{}

	//Move assignment operator
	HDelegateBase& operator=(HDelegateBase&& other) noexcept
	{
		Release();
		m_Allocator = std::move(other.m_Allocator);
		return *this;
	}

	//Gets the owner of the deletage
	//Only valid for SPDelegate and RawDelegate.
	//Otherwise returns nullptr by default
	const void* GetOwner() const
	{
		if (m_Allocator.HasAllocation())
		{
			return GetDelegate()->GetOwner();
		}
		return nullptr;
	}

	size_t GetSize() const
	{
		return m_Allocator.GetSize();
	}

	//Clear the bound delegate if it is bound to the given object.
	//Ignored when pObject is a nullptr
	void ClearIfBoundTo(void* pObject)
	{
		if (pObject != nullptr && IsBoundTo(pObject))
		{
			Release();
		}
	}

	//Clear the bound delegate if it exists
	void Clear()
	{
		Release();
	}

	//If the allocator has a size, it means it's bound to something
	bool IsBound() const
	{
		return m_Allocator.HasAllocation();
	}

	bool IsBoundTo(void* pObject) const
	{
		if (pObject == nullptr || m_Allocator.HasAllocation() == false)
		{
			return false;
		}
		return GetDelegate()->GetOwner() == pObject;
	}

protected:
	void Release()
	{
		if (m_Allocator.HasAllocation())
		{
			GetDelegate()->~IDelegateInstanceBase();
			m_Allocator.Free();
		}
	}

	IDelegateInstanceBase* GetDelegate() const
	{
		return static_cast<IDelegateInstanceBase*>(m_Allocator.GetAllocation());
	}

	//Allocator for the delegate itself.
	//Delegate gets allocated when its is smaller or equal than 64 bytes in size.
	//Can be changed by preference
	HInlineAllocator<DELEGATE_INLINE_ALLOCATION_SIZE> m_Allocator;
};

//Delegate that can be bound to by just ONE object
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

	//Create delegate using member function
	template<typename T, typename... Args2>
	NO_DISCARD static HDelegate CreateRaw(T* pObj, NonConstMemberFunction<T, Args2...> pFunction, Args2... args)
	{
		HDelegate handler;
		handler.Bind<PRawDelegate<false, T, RetVal(Args...), Args2...>>(pObj, pFunction, std::forward<Args2>(args)...);
		return handler;
	}

	template<typename T, typename... Args2>
	NO_DISCARD static HDelegate CreateRaw(T* pObj, ConstMemberFunction<T, Args2...> pFunction, Args2... args)
	{
		HDelegate handler;
		handler.Bind<PRawDelegate<true, T, RetVal(Args...), Args2...>>(pObj, pFunction, std::forward<Args2>(args)...);
		return handler;
	}

	//Create delegate using global/static function
	template<typename... Args2>
	NO_DISCARD static HDelegate CreateStatic(RetVal(*pFunction)(Args..., Args2...), Args2... args)
	{
		HDelegate handler;
		handler.Bind<PStaticDelegate<RetVal(Args...), Args2...>>(pFunction, std::forward<Args2>(args)...);
		return handler;
	}

	//Create delegate using std::shared_ptr
	template<typename T, typename... Args2>
	NO_DISCARD static HDelegate CreateSP(const PSharedPtr<T>& pObject, NonConstMemberFunction<T, Args2...> pFunction, Args2... args)
	{
		HDelegate handler;
		handler.Bind<PSPDelegate<false, T, RetVal(Args...), Args2...>>(pObject, pFunction, std::forward<Args2>(args)...);
		return handler;
	}

	template<typename T, typename... Args2>
	NO_DISCARD static HDelegate CreateSP(const PSharedPtr<T>& pObject, ConstMemberFunction<T, Args2...> pFunction, Args2... args)
	{
		HDelegate handler;
		handler.Bind<PSPDelegate<true, T, RetVal(Args...), Args2...>>(pObject, pFunction, std::forward<Args2>(args)...);
		return handler;
	}

	//Create delegate using a lambda
	template<typename TLambda, typename... Args2>
	NO_DISCARD static HDelegate CreateLambda(TLambda&& lambda, Args2... args)
	{
		HDelegate handler;
		handler.Bind<PLambdaDelegate<TLambda, RetVal(Args...), Args2...>>(std::forward<TLambda>(lambda), std::forward<Args2>(args)...);
		return handler;
	}

	//Bind a member function
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

	//Bind a static/global function
	template<typename... Args2>
	void BindStatic(RetVal(*pFunction)(Args..., Args2...), Args2&&... args)
	{
		*this = CreateStatic<Args2... >(pFunction, std::forward<Args2>(args)...);
	}

	//Bind a lambda
	template<typename LambdaType, typename... Args2>
	void BindLambda(LambdaType&& lambda, Args2&&... args)
	{
		*this = CreateLambda<LambdaType, Args2... >(std::forward<LambdaType>(lambda), std::forward<Args2>(args)...);
	}

	//Bind a member function with a shared_ptr object
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

	//Execute the delegate with the given parameters
	RetVal Execute(Args... args) const
	{
		DELEGATE_ASSERT(m_Allocator.HasAllocation(), "Delegate is not bound");
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
		void* pAlloc = m_Allocator.Allocate(sizeof(T));
		new (pAlloc) T(std::forward<Args3>(args)...);
	}
};

//Delegate that can be bound to by MULTIPLE objects
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
	//Default constructor
	constexpr HMulticastDelegate()
		: m_Locks(0)
	{
	}

	//Default destructor
	~HMulticastDelegate() noexcept = default;

	//Default copy constructor
	HMulticastDelegate(const HMulticastDelegate& other) = default;

	//Defaul copy assignment operator
	HMulticastDelegate& operator=(const HMulticastDelegate& other) = default;

	//Move constructor
	HMulticastDelegate(HMulticastDelegate&& other) noexcept
		: m_Events(std::move(other.m_Events)),
		m_Locks(std::move(other.m_Locks))
	{
	}

	//Move assignment operator
	HMulticastDelegate& operator=(HMulticastDelegate&& other) noexcept
	{
		m_Events = std::move(other.m_Events);
		m_Locks = std::move(other.m_Locks);
		return *this;
	}

	template<typename T>
	HDelegateHandle operator+=(T&& l)
	{
		return Add(DelegateT::CreateLambda(std::move(l)));
	}

	//Add delegate with the += operator
	HDelegateHandle operator+=(DelegateT&& handler) noexcept
	{
		return Add(std::forward<DelegateT>(handler));
	}

	//Remove a delegate using its DelegateHandle
	bool operator-=(HDelegateHandle& handle)
	{
		return Remove(handle);
	}

	HDelegateHandle Add(DelegateT&& handler) noexcept
	{
		//Favour an empty space over a possible array reallocation
		for (size_t i = 0; i < m_Events.size(); ++i)
		{
			if (m_Events[i].Handle.IsValid() == false)
			{
				m_Events[i] = DelegateHandlerPair(HDelegateHandle(true), std::move(handler));
				return m_Events[i].Handle;
			}
		}
		m_Events.emplace_back(HDelegateHandle(true), std::move(handler));
		return m_Events.back().Handle;
	}

	//Bind a member function
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

	//Bind a static/global function
	template<typename... Args2>
	HDelegateHandle AddStatic(void(*pFunction)(Args..., Args2...), Args2&&... args)
	{
		return Add(DelegateT::CreateStatic(pFunction, std::forward<Args2>(args)...));
	}

	//Bind a lambda
	template<typename LambdaType, typename... Args2>
	HDelegateHandle AddLambda(LambdaType&& lambda, Args2&&... args)
	{
		return Add(DelegateT::CreateLambda(std::forward<LambdaType>(lambda), std::forward<Args2>(args)...));
	}

	//Bind a member function with a shared_ptr object
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

	//Removes all handles that are bound from a specific object
	//Ignored when pObject is null
	//Note: Only works on Raw and SP bindings
	void RemoveObject(void* pObject)
	{
		if (pObject != nullptr)
		{
			for (size_t i = 0; i < m_Events.size(); ++i)
			{
				if (m_Events[i].Callback.GetOwner() == pObject)
				{
					if (IsLocked())
					{
						m_Events[i].Callback.Clear();
					}
					else
					{
						std::swap(m_Events[i], m_Events[m_Events.size() - 1]);
						m_Events.pop_back();
					}
				}
			}
		}
	}

	//Remove a function from the event list by the handle
	bool Remove(HDelegateHandle& handle)
	{
		if (handle.IsValid())
		{
			for (size_t i = 0; i < m_Events.size(); ++i)
			{
				if (m_Events[i].Handle == handle)
				{
					if (IsLocked())
					{
						m_Events[i].Callback.Clear();
					}
					else
					{
						std::swap(m_Events[i], m_Events[m_Events.size() - 1]);
						m_Events.pop_back();
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
			for (size_t i = 0; i < m_Events.size(); ++i)
			{
				if (m_Events[i].Handle == handle)
				{
					return true;
				}
			}
		}
		return false;
	}

	//Remove all the functions bound to the delegate
	void RemoveAll()
	{
		if (IsLocked())
		{
			for (DelegateHandlerPair& handler : m_Events)
			{
				handler.Callback.Clear();
			}
		}
		else
		{
			m_Events.clear();
		}
	}

	void Compress(const size_t maxSpace = 0)
	{
		if (IsLocked() == false)
		{
			size_t toDelete = 0;
			for (size_t i = 0; i < m_Events.size() - toDelete; ++i)
			{
				if (m_Events[i].Handle.IsValid() == false)
				{
					std::swap(m_Events[i], m_Events[toDelete]);
					++toDelete;
				}
			}
			if (toDelete > maxSpace)
			{
				m_Events.resize(m_Events.size() - toDelete);
			}
		}
	}

	//Execute all functions that are bound
	void BroadCast(Args ...args)
	{
		Lock();
		for (size_t i = 0; i < m_Events.size(); ++i)
		{
			if (m_Events[i].Handle.IsValid())
			{
				m_Events[i].Callback.Execute(std::forward<Args>(args)...);
			}
		}
		Unlock();
	}

	size_t GetSize() const
	{
		return m_Events.size();
	}

private:
	void Lock()
	{
		++m_Locks;
	}

	void Unlock()
	{
		//Unlock() should never be called more than Lock()!
		DELEGATE_ASSERT(m_Locks > 0);
		--m_Locks;
	}

	//Returns true is the delegate is currently broadcasting
	//If this is true, the order of the array should not be changed otherwise this causes undefined behaviour
	bool IsLocked() const
	{
		return m_Locks > 0;
	}

	std::vector<DelegateHandlerPair> m_Events;
	unsigned int m_Locks;
};




//template<class RetType, class T, class ... Args >
//using HDelegateMemberNonConstFunction = RetType(T::*)(Args...);
//
//template<class RetType, class T, class ... Args >
//using HDelegateMemberConstFunction = RetType(T::*)(Args...) const;
//
//template<class RetType, class ... Args >
//using HDelegateStaticFunction = RetType(*)(Args...);
//
//class IDelegateInstanceBase : public IMemoryObject
//{
//public:
//	virtual ~IDelegateInstanceBase() = default;
//
//	virtual bool IsBound() const = 0;
//	virtual JGType GetType() const = 0;
//};
//
//template<class RetType, class ... Args>
//class IDelegateInstance : public IDelegateInstanceBase
//{
//public:
//	virtual ~IDelegateInstance() = default;
//	virtual JGType GetType() const { return JGType::GenerateType<IDelegateInstance<RetType, Args...>>(); }
//	virtual bool IsBound() const override { return false; }
//	virtual RetType Execute(Args ... args) const { return RetType(); }
//	virtual void Reset() {}
//};
//
//template<class FuncType, class T, class RetType, class ... Args2>
//class PDelegateInstance;
//
//template<class FuncType, class T, class RetType, class ... Args, class ... Args2>
//class PDelegateInstance<FuncType, T, RetType(Args...), Args2...> : public IDelegateInstance<RetType, Args...>
//{
//	FuncType _func;
//	PWeakPtr<T> _refObject;
//	HTuple<Args2...> _payload;
//public:
//	PDelegateInstance() = default;
//	PDelegateInstance(PWeakPtr<T> inRefObject, FuncType inFunc, HTuple<Args2...> inPayload)
//		: _refObject(inRefObject), _func(inFunc), _payload(inPayload) {}
//	virtual ~PDelegateInstance() = default;
//
//	void Bind(PWeakPtr<T> ptr, FuncType func, Args2&&... args)
//	{
//		Reset();
//		_refObject = ptr;
//		_func = func;
//		_payload = HTuple<Args2...>(std::forward<Args2>(args)...);
//	}
//
//	virtual bool IsBound() const override
//	{
//		if (_refObject.IsValid() == false)
//		{
//			return false;
//		}
//
//		return true;
//	}
//	
//	virtual RetType Execute(Args&& ... args) const override
//	{
//		return ExecuteInternal(std::forward<Args>(args)..., std::index_sequence_for<Args2...>());
//	}
//			
//	virtual void Reset() override
//	{
//		_refObject.Reset();
//		_refObject = nullptr;
//	}
//private:
//	template<uint64... Is>
//	RetType ExecuteInternal(Args&&... args, std::index_sequence<Is...>) const
//	{
//		return _func(std::forward<Args>(args)..., std::get<Is>(_payload)...);
//	}
//};
//
//template<class FuncType, class T, class RetType, class ... Args2>
//class PRawDelegateInstance;
//
//template<class FuncType, class T, class RetType, class ... Args, class ... Args2>
//class PRawDelegateInstance<FuncType, T, RetType(Args...), Args2...> : public IDelegateInstance<RetType, Args...>
//{
//	FuncType _func;
//	const T* _refObject = nullptr;
//	HTuple<Args2...> _payload;
//public:
//	PRawDelegateInstance() = default;
//	PRawDelegateInstance(const T* inRefObject, FuncType inFunc, HTuple<Args2...> inPayload)
//		: _refObject(inRefObject), _func(inFunc), _payload(inPayload) {}
//
//	virtual ~PRawDelegateInstance() = default;
//
//	void Bind(const T* ptr, FuncType func, Args2&&... args)
//	{
//		Reset();
//		_refObject = ptr;
//		_func = func;
//		_payload = HTuple<Args2...>(std::forward<Args2>(args)...);
//	}
//
//	virtual bool IsBound() const override
//	{
//		if (_refObject == nullptr)
//		{
//			return false;
//		}
//
//		return true;
//	}
//
//	virtual RetType Execute(Args ... args) const override
//	{
//		return ExecuteInternal(std::forward<Args>(args)..., std::index_sequence_for<Args2...>());
//	}
//
//	virtual void Reset() override
//	{
//		_refObject = nullptr;
//	}
//private:
//	template<uint64... Is>
//	RetType ExecuteInternal(Args&&... args, std::index_sequence<Is...>) const
//	{
//		return _func(std::forward<Args>(args)..., std::get<Is>(_payload)...);
//	}
//};
//
//
//template<class RetType, class ... Args2>
//class PStaticDelegateInstance;
//
//template<class RetType, class ... Args, class ... Args2>
//class PStaticDelegateInstance<RetType(Args...), Args2...> : public IDelegateInstance<RetType, Args...>
//{
//	HDelegateStaticFunction<RetType, Args..., Args2...> _func;
//	HTuple<Args2...> _payload;
//public:
//	PStaticDelegateInstance() = default;
//	PStaticDelegateInstance(HDelegateStaticFunction<RetType, Args..., Args2...> inFunc, HTuple<Args2...> inPayload)
//		: _func(inFunc), _payload(inPayload) {}
//
//	virtual ~PStaticDelegateInstance() = default;
//
//	void Bind(HDelegateStaticFunction<RetType, Args..., Args2...> func, Args2&&... args)
//	{
//		Reset();
//		_func = func;
//		_payload = HTuple<Args2...>(std::forward<Args2>(args)...);
//	}
//
//	virtual bool IsBound() const override
//	{
//		if (_func == nullptr)
//		{
//			return false;
//		}
//
//		return true;
//	}
//
//	virtual RetType Execute(Args ... args) const override
//	{
//		return ExecuteInternal(std::forward<Args>(args)..., std::index_sequence_for<Args2...>());
//	}
//
//	virtual void Reset() override
//	{
//		_func = nullptr;
//	}
//private:
//	template<uint64... Is>
//	RetType ExecuteInternal(Args&&... args, std::index_sequence<Is...>) const
//	{
//		return _func(std::forward<Args>(args)..., std::get<Is>(_payload)...);
//	}
//};
//
//
//template<class RetType, class ...Args>
//class HDelgate
//{
//	PSharedPtr<IDelegateInstance<RetType, Args...>> DelegateInstance = nullptr;
//public:
//	virtual ~HDelgate() = default;
//
//	bool IsBound() const
//	{
//		if (DelegateInstance == nullptr)
//		{
//			return false;
//		}
//
//		return DelegateInstance->IsBound();
//	}
//
//	RetType Execute(Args&& ... args) const
//	{
//		if (DelegateInstance == nullptr)
//		{
//			return RetType();
//		}
//
//		return DelegateInstance->Execute(std::forward<Args>(args)...);
//	}
//
//	RetType ExecuteIfBound(Args... args) const
//	{
//		if (IsBound() == false)
//		{
//			return RetType();
//		}
//
//		return Execute(std::forward<Args>(args)...);
//	}
//
//	void Reset()
//	{
//		if (DelegateInstance == nullptr)
//		{
//			return;
//		}
//		DelegateInstance->Reset();
//		DelegateInstance = nullptr;
//	}
//
//	//template<class TLambda, class T, class ...Args2>
//	//static HDelgate<RetType, Args...> Create(PSharedPtr<T> ptr, const TLambda& func, Args2&&... args)
//	//{
//	//	PSharedPtr<PDelegateInstance<TLambda, T, RetType(Args...), Args2...>> Instance = Allocate<PDelegateInstance<TLambda, T, RetType(Args...), Args2...>>();
//	//	Instance->Bind(ptr, func, std::forward<Args2>(args)...);
//
//	//	HDelgate<RetType, Args...> Result;
//	//	Result.DelegateInstance = Instance;
//	//	return Result;
//	//}
//
//	//template<class TLambda, class T, class ...Args2>
//	//static HDelgate<RetType, Args...> Create(const T* ptr, const HDelegateMemberNonConstFunction<RetType, T, Args..., Args2...>& func, Args2&&... args)
//	//{
//	//	return Create(SharedWrap(ptr), func, std::forward<Args2>(args)...);
//	//}
//
//	template<class T, class ...Args2>
//	static HDelgate<RetType, Args...> Create(PSharedPtr<T> ptr, HDelegateMemberNonConstFunction<RetType, T, Args..., Args2...> func, Args2&&... args)
//	{
//		PSharedPtr<PDelegateInstance<HDelegateMemberNonConstFunction<RetType, T, Args..., Args2...>, T, RetType(Args...), Args2...>> Instance = Allocate<PDelegateInstance<HDelegateMemberNonConstFunction<RetType, T, Args..., Args2...>, T, RetType(Args...), Args2...>>();
//		Instance->Bind(ptr, func, std::forward<Args2>(args)...);
//
//		HDelgate<RetType, Args...> Result;
//		Result.DelegateInstance = Instance;
//		return Result;
//	}
//
//	template<class T, class ...Args2>
//	static HDelgate<RetType, Args...> Create(const T* ptr, HDelegateMemberNonConstFunction<RetType, T, Args..., Args2...> func, Args2&&... args)
//	{
//		PSharedPtr<PRawDelegateInstance<HDelegateMemberNonConstFunction<RetType, T, Args..., Args2...>, T, RetType(Args...), Args2...>> Instance = Allocate<PRawDelegateInstance<HDelegateMemberNonConstFunction<RetType, T, Args..., Args2...>, T, RetType(Args...), Args2...>>();
//		Instance->Bind(ptr, func, std::forward<Args2>(args)...);
//
//		HDelgate<RetType, Args...> Result;
//		Result.DelegateInstance = Instance;
//		return Result;
//	}
//
//	template<class T, class ...Args2>
//	static HDelgate<RetType, Args...> Create(PSharedPtr<T> ptr, HDelegateMemberConstFunction<RetType, T, Args..., Args2...> func, Args2&&... args)
//	{
//		PSharedPtr<PDelegateInstance<HDelegateMemberConstFunction<RetType, T, Args..., Args2...>, T, RetType(Args...), Args2...>> Instance = Allocate<PDelegateInstance<HDelegateMemberConstFunction<RetType, T, Args..., Args2...>, T, RetType(Args...), Args2...>>();
//		Instance->Bind(ptr, func, std::forward<Args2>(args)...);
//
//		HDelgate<RetType, Args...> Result;
//		Result.DelegateInstance = Instance;
//		return Result;
//	}
//
//	template<class T, class ...Args2>
//	static HDelgate<RetType, Args...> Create(const T* ptr, HDelegateMemberConstFunction<RetType, T, Args..., Args2...> func, Args2&&... args)
//	{
//		PSharedPtr<PRawDelegateInstance<HDelegateMemberConstFunction<RetType, T, Args..., Args2...>, T, RetType(Args...), Args2...>> Instance = Allocate<PRawDelegateInstance<HDelegateMemberConstFunction<RetType, T, Args..., Args2...>, T, RetType(Args...), Args2...>>();
//		Instance->Bind(ptr, func, std::forward<Args2>(args)...);
//
//		HDelgate<RetType, Args...> Result;
//		Result.DelegateInstance = Instance;
//		return Result;
//	}
//
//	template<class ...Args2>
//	static HDelgate<RetType, Args...> Create(HDelegateStaticFunction<RetType, Args..., Args2...> func, Args2&&... args)
//	{
//		PSharedPtr<PStaticDelegateInstance<RetType(Args...), Args2...>> Instance = Allocate<PStaticDelegateInstance<RetType(Args...), Args2...>>();
//		Instance->Bind(func, std::forward<Args2>(args)...);
//
//		HDelgate<RetType, Args...> Result;
//		Result.DelegateInstance = Instance;
//		return Result;
//	}
//
//	template<class TLambda, class T, class ...Args2>
//	static HDelgate<RetType, Args...> CreateLambda(PSharedPtr<T> ptr, TLambda func, Args2&&... args)
//	{
//		PSharedPtr<PDelegateInstance<TLambda, T, RetType(Args...), Args2...>> Instance = Allocate<PDelegateInstance<TLambda, T, RetType(Args...), Args2...>>();
//		Instance->Bind(ptr, func, std::forward<Args2>(args)...);
//
//		HDelgate<RetType, Args...> Result;
//		Result.DelegateInstance = Instance;
//		return Result;
//	}
//
//	template<class TLambda, class T, class ...Args2>
//	static HDelgate<RetType, Args...> CreateLambda(const T* ptr, TLambda func, Args2&&... args)
//	{
//		PSharedPtr<PRawDelegateInstance<TLambda, T, RetType(Args...), Args2...>> Instance = Allocate<PRawDelegateInstance<TLambda, T, RetType(Args...), Args2...>>();
//		Instance->Bind(ptr, func, std::forward<Args2>(args)...);
//
//		HDelgate<RetType, Args...> Result;
//		Result.DelegateInstance = Instance;
//		return Result;
//	}
//};
//
//template<class ... Args>
//class HMultiDelegate
//{
//	HList<HDelgate<void, Args...>> _delegates;
//	HList<void*> _delegateKeys;
//public:
//	template<class T, class ...Args2>
//	void Add(PSharedPtr<T> ptr, HDelegateMemberConstFunction<void, T, Args..., Args2...> func, Args2&& ... args)
//	{
//		_delegates.push_back(HDelgate<void, Args...>::Create(ptr, func, std::forward<Args2>(args)...));
//		_delegateKeys.push_back((void*)(ptr.Pin().GetRawPointer()));
//	}
//
//	template<class T, class ...Args2>
//	void Add(PSharedPtr<T> ptr, HDelegateMemberNonConstFunction<void, T, Args..., Args2...> func, Args2&& ... args)
//	{
//		_delegates.push_back(HDelgate<void, Args...>::Create(ptr, func, std::forward<Args2>(args)...));
//		_delegateKeys.push_back((void*)(ptr.GetRawPointer()));
//	}
//
//	template<class TLambda, class T, class ...Args2>
//	void AddLambda(PSharedPtr<T> ptr, TLambda func, Args2&& ... args)
//	{
//		_delegates.push_back(HDelgate<void, Args...>::CreateLambda(ptr, func, std::forward<Args2>(args)...));
//		_delegateKeys.push_back((void*)(ptr.GetRawPointer()));
//	}
//
//	void Remove(void* ptr)
//	{
//		if (ptr == nullptr)
//		{
//			return;
//		}
//
//		for (uint64 i = 0; i < _delegates.size();)
//		{
//			if (_delegates[i].IsBound() == false)
//			{
//				removeInternal(i);
//			}
//			else if (_delegateKeys[i] == ptr)
//			{
//				removeInternal(i);
//			}
//			else ++i;
//		}
//	}
//
//	void BroadCast(Args... args)
//	{
//		for (uint64 i = 0; i < _delegates.size();)
//		{
//			if (_delegates[i].IsBound() == false)
//			{
//				removeInternal((int32)i);
//			}
//			else
//			{
//				_delegates[i].Execute(args...);
//				++i;
//			}
//		}
//	}
//
//private:
//	void removeInternal(int32 index)
//	{
//		uint64 len = _delegates.size();
//
//		HDelgate<void, Args...> tempDelegate = _delegates[len - 1];
//
//		_delegates[len - 1] = _delegates[index];
//		_delegates[index] = tempDelegate;
//		_delegates.pop_back();
//
//		void* tempKey = _delegateKeys[len - 1];
//		_delegateKeys[len - 1] = _delegateKeys[index];
//		_delegateKeys[index] = tempKey;
//		_delegateKeys.pop_back();
//	}
//};









//#define JG_DECLARE_DELEGATE(DelegateName) \
//using DelegateName = HDelgate<void>;\
//
//#define JG_DECLARE_DELEGATE_ONEPARAM(DelegateName, OneParam)\
//using DelegateName   = HDelgate<void, ##OneParam>;\
//
//#define JG_DECLARE_DELEGATE_TWOPARAM(DelegateName, OneParam, TwoParam) \
//using DelegateName   = HDelgate<void, ##OneParam, ##TwoParam>;\
//
//#define JG_DECLARE_DELEGATE_THREEPARAM(DelegateName, OneParam, TwoParam, ThreeParam) \
//using DelegateName   = HDelgate<void, ##OneParam, ##TwoParam, ##ThreeParam>; \
//
//#define JG_DECLARE_DELEGATE_FOURPARAM(DelegateName, OneParam, TwoParam, ThreeParam, FourParam) \
//using DelegateName   = HDelgate<void, ##OneParam, ##TwoParam, ##ThreeParam, ##FourParam>; \
//
//#define JG_DECLARE_DELEGATE_FIVEPARAM(DelegateName, OneParam, TwoParam, ThreeParam, FourParam, FiveParam) \
//using DelegateName   = HDelgate<void, ##OneParam, ##TwoParam, ##ThreeParam, ##FourParam, ##FiveParam>; \

//
//#define JG_DECLARE_DELEGATE_RETVAL(RetType, DelegateName) \
//using DelegateName = HDelgate<##RetType>;\
//
//#define JG_DECLARE_DELEGATE_ONEPARAM_RETVAL(RetType, DelegateName, OneParam)\
//using DelegateName   = HDelgate<##RetType, ##OneParam>;\
//
//#define JG_DECLARE_DELEGATE_TWOPARAM_RETVAL(RetType, DelegateName, OneParam, TwoParam) \
//using DelegateName   = HDelgate<##RetType, ##OneParam, ##TwoParam>;\
//
//#define JG_DECLARE_DELEGATE_THREEPARAM_RETVAL(RetType, DelegateName, OneParam, TwoParam, ThreeParam) \
//using DelegateName   = HDelgate<##RetType, ##OneParam, ##TwoParam, ##ThreeParam>; \
//
//#define JG_DECLARE_DELEGATE_FOURPARAM_RETVAL(RetType, DelegateName, OneParam, TwoParam, ThreeParam, FourParam) \
//using DelegateName   = HDelgate<##RetType, ##OneParam, ##TwoParam, ##ThreeParam, ##FourParam>; \
//
//#define JG_DECLARE_DELEGATE_FIVEPARAM_RETVAL(RetType, DelegateName, OneParam, TwoParam, ThreeParam, FourParam, FiveParam) \
//using DelegateName   = HDelgate<##RetType, ##OneParam, ##TwoParam, ##ThreeParam, ##FourParam, ##FiveParam>; \
//
//
//#define JG_DECLARE_MULTI_DELEGATE(DelegateName) \
//using DelegateName = HMultiDelegate<>;\
//
//#define JG_DECLARE_MULTI_DELEGATE_ONEPARAM(DelegateName, OneParam)\
//using DelegateName   = HMultiDelegate<##OneParam>;\
//
//#define JG_DECLARE_MULTI_DELEGATE_TWOPARAM(DelegateName, OneParam, TwoParam) \
//using DelegateName   = HMultiDelegate<##OneParam, ##TwoParam>;\
//
//#define JG_DECLARE_MULTI_DELEGATE_THREEPARAM(DelegateName, OneParam, TwoParam, ThreeParam) \
//using DelegateName   = HMultiDelegate<##OneParam, ##TwoParam, ##ThreeParam>; \
//
//#define JG_DECLARE_MULTI_DELEGATE_FOURPARAM(DelegateName, OneParam, TwoParam, ThreeParam, FourParam) \
//using DelegateName   = HMultiDelegate<##OneParam, ##TwoParam, ##ThreeParam, ##FourParam>; \
//
//#define JG_DECLARE_MULTI_DELEGATE_FIVEPARAM(DelegateName, OneParam, TwoParam, ThreeParam, FourParam, FiveParam) \
//using DelegateName   = HMultiDelegate<##OneParam, ##TwoParam, ##ThreeParam, ##FourParam, ##FiveParam>; \
//
//#define JG_DELEGATE_FN_BIND(fn) &##fn //std::bind(&##fn, this)
//#define JG_DELEGATE_FN_BIND_ONEPARAM(fn) &##fn //std::bind(&##fn, this, std::placeholders::_1)
//#define JG_DELEGATE_FN_BIND_TWOPARAM(fn) &##fn //std::bind(&##fn, this, std::placeholders::_1, std::placeholders::_2)
//#define JG_DELEGATE_FN_BIND_THREEPARAM(fn) &##fn //std::bind(&##fn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
//#define JG_DELEGATE_FN_BIND_FOURPARAM(fn) &##fn //std::bind(&##fn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
//#define JG_DELEGATE_FN_BIND_FIVEPARAM(fn) &##fn //std::bind(&##fn, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)
//
