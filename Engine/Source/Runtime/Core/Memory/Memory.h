#pragma once

#include "CoreDefines.h"
#include "CoreSystem.h"

class IMemoryObject
{
protected:
	IMemoryObject() = default;
public:
	virtual ~IMemoryObject() = default;
	
protected:
	friend class GMemoryGlobalSystem;
	virtual void Construct() {}
	virtual void Destruction() {}
};

class PMemoryObject : public IMemoryObject
{
public:
	virtual ~PMemoryObject() = default;
};

template<class T>
class PWeakPtr;

template<class T>
class PSharedPtr;

namespace PMemoryPrivate
{
	template<class T>
	class PTemporaryOwner : public IMemoryObject
	{
	public:
		T* ptr = nullptr;

		HAtomicInt32* pRefCount = nullptr;
		HAtomicInt32* pWeakCount = nullptr;

		PTemporaryOwner(const PSharedPtr<T>& sharedPtr)
			: ptr(sharedPtr._ptr)
			, pRefCount(sharedPtr._pRefCount)
			, pWeakCount(sharedPtr._pWeakCount)
		{}

		PTemporaryOwner(const PWeakPtr<T>& weakPtr)
			: ptr(weakPtr._ptr)
			, pRefCount(weakPtr._pRefCount)
			, pWeakCount(weakPtr._pWeakCount)
		{}

		PTemporaryOwner(PSharedPtr<T>&& sharedPtr)
			: ptr(sharedPtr._ptr)
			, pRefCount(sharedPtr._pRefCount)
			, pWeakCount(sharedPtr._pWeakCount)
		{
			sharedPtr._ptr = nullptr;
			sharedPtr._pRefCount = nullptr;
			sharedPtr._pWeakCount = nullptr;
		}
		
		PTemporaryOwner(PWeakPtr<T>&& weakPtr)
			: ptr(weakPtr._ptr)
			, pRefCount(weakPtr._pRefCount)
			, pWeakCount(weakPtr._pWeakCount)
		{
			weakPtr._ptr = nullptr;
			weakPtr._pRefCount  = nullptr;
			weakPtr._pWeakCount = nullptr;
		}

		virtual ~PTemporaryOwner() = default;
	};
};

template<class T>
class TRawPtr
{
public:

};


template<class T>
class PSharedPtr : public IMemoryObject
{
private:
	friend class GMemoryGlobalSystem;
	friend PMemoryPrivate::PTemporaryOwner<T>;
	friend PWeakPtr<T>;

	T* _ptr = nullptr;

	HAtomicInt32* _pRefCount = nullptr;
	HAtomicInt32* _pWeakCount = nullptr;

public:
	PSharedPtr()  = default;
	PSharedPtr(nullptr_t) {};

	template <class U, std::enable_if<std::is_base_of<T, U>::value, int32>::type = 0>
	PSharedPtr(const PSharedPtr<U>& rhs)
	{
		copy<U>(rhs);
	}

	PSharedPtr(const PSharedPtr<T>& rhs)
	{
		copy<T>(rhs);
	}

	template <class U, std::enable_if<std::is_base_of<T, U>::value, int32>::type = 0>
	PSharedPtr(PSharedPtr<U>&& rhs)
	{
		move<U>(std::move(rhs));
	}

	PSharedPtr(PSharedPtr<T>&& rhs)
	{
		move<T>(std::move(rhs));
	}

	virtual ~PSharedPtr()
	{
		if (IsValid() == false)
		{
			return;
		}
		subRefCount();
	}
public:
	template <class U, std::enable_if<std::is_base_of<T, U>::value, int32>::type = 0>
	PSharedPtr<T>& operator=(const PSharedPtr<U>& rhs)
	{
		copy<U>(rhs);

		return *this;
	}

	PSharedPtr<T>& operator=(const PSharedPtr<T>& rhs)
	{
		copy<T>(rhs);
		
		return *this;
	}

	template <class U, std::enable_if<std::is_base_of<T, U>::value, int32>::type = 0>
	PSharedPtr<T>& operator=(PSharedPtr<U>&& rhs)
	{
		move<U>(std::move(rhs));

		return *this;
	}

	PSharedPtr<T>& operator=(PSharedPtr<T>&& rhs)
	{
		move<T>(std::move(rhs));

		return *this;
	}

	bool operator==(const PSharedPtr<T>& ptr) const
	{
		return _ptr == ptr._ptr;
	}

	bool operator!=(const PSharedPtr<T>& ptr) const
	{
		return _ptr != ptr._ptr;
	}

	bool operator==(nullptr_t) const
	{
		return _ptr == nullptr;
	}

	bool operator!=(nullptr_t) const
	{
		return _ptr != nullptr;
	}

	T* operator->()
	{
		return _ptr;
	}

	const T* operator->() const
	{
		return _ptr;
	}

	T& operator*()
	{
		return *_ptr;
	}

	const T& operator*() const
	{
		return *_ptr;
	}
public:
	bool IsUnique() const
	{
		if (IsValid() == false)
		{
			return false;
		}
		return _pRefCount->load() == 1;
	}

	bool IsValid() const 
	{
		return _ptr != nullptr;
	}

	T* GetRawPointer() const
	{
		return _ptr;
	}

	const T* GetRawConstPointer() const
	{
		return _ptr;
	}

	void Reset()
	{
		if (IsValid() == true)
		{
			subRefCount();
			_ptr = nullptr;
			_pRefCount  = nullptr;
			_pWeakCount = nullptr;
		}
	}
private:
	template<class U>
	void copy(const PSharedPtr<U>& rhs)
	{
		if (rhs.IsValid() == false)
		{
			return;
		}

		Reset();

		PMemoryPrivate::PTemporaryOwner<U> owner(rhs);

		_ptr = owner.ptr;
		_pRefCount  = owner.pRefCount;
		_pWeakCount = owner.pWeakCount;

		addRefCount();
	}

	template<class U>
	void move(PSharedPtr<U>&& rhs)
	{
		if (rhs.IsValid() == false)
		{
			return;
		}

		Reset();

		PMemoryPrivate::PTemporaryOwner<U> owner(std::move(rhs));

		_ptr = owner.ptr;
		_pRefCount  = owner.pRefCount;
		_pWeakCount = owner.pWeakCount;
	}

	void addRefCount()
	{
		if (_pRefCount == nullptr)
		{
			return;
		}

		_pRefCount->fetch_add(1);
	}

	void subRefCount()
	{
		if (_pRefCount == nullptr)
		{
			return;
		}

		_pRefCount->fetch_sub(1);
	}
};

template<class T>
class PWeakPtr : public IMemoryObject
{
	friend PMemoryPrivate::PTemporaryOwner<T>;
	friend class GMemoryGlobalSystem;

	T* _ptr = nullptr;

	HAtomicInt32* _pRefCount   = nullptr;
	HAtomicInt32* _pWeakCount  = nullptr;

public:
	PWeakPtr() = default;

	template <class U, std::enable_if<std::is_base_of<T, U>::value, int32>::type = 0>
	PWeakPtr(const PWeakPtr<U>& rhs)
	{
		copy<U>(rhs);
	}

	PWeakPtr(const PWeakPtr<T>& rhs)
	{
		copy<T>(rhs);
	}

	template <class U, std::enable_if<std::is_base_of<T, U>::value, int32>::type = 0>
	PWeakPtr(PWeakPtr<U>&& rhs)
	{
		move<U>(std::move(rhs));
	}

	PWeakPtr(PWeakPtr<T>&& rhs)
	{
		move<T>(std::move(rhs));
	}

	template <class U, std::enable_if<std::is_base_of<T, U>::value, int32>::type = 0>
	PWeakPtr(const PSharedPtr<U>& rhs)
	{
		set<U>(rhs);
	}

	PWeakPtr(const PSharedPtr<T>& rhs)
	{
		set<T>(rhs);
	}

	template <class U, std::enable_if<std::is_base_of<T, U>::value, int32>::type = 0>
	PWeakPtr(PSharedPtr<U>&& rhs)
	{
		set<U>(rhs);
	}

	PWeakPtr(PSharedPtr<T>&& rhs)
	{
		set<T>(rhs);
	}

	virtual ~PWeakPtr()
	{
		if (IsValid() == false)
		{
			return;
		}

		subWeakCount();
	}
public:
	template <class U, std::enable_if<std::is_base_of<T, U>::value, int32>::type = 0>
	PWeakPtr<T>& operator=(const PWeakPtr<U>& rhs)
	{
		copy<U>(rhs);

		return *this;
	}

	PWeakPtr<T>& operator=(const PWeakPtr<T>& rhs)
	{
		copy<T>(rhs);

		return *this;
	}

	template <class U, std::enable_if<std::is_base_of<T, U>::value, int32>::type = 0>
	PWeakPtr<T>& operator=(PWeakPtr<U>&& rhs)
	{
		move<U>(std::move(rhs));

		return *this;
	}

	PWeakPtr<T>& operator=(PWeakPtr<T>&& rhs)
	{
		move<T>(std::move(rhs));

		return *this;
	}

	template <class U, std::enable_if<std::is_base_of<T, U>::value, int32>::type = 0>
	PWeakPtr<T>& operator=(const PSharedPtr<U>& rhs)
	{
		set<U>(rhs);

		return *this;
	}

	PWeakPtr<T>& operator=(const PSharedPtr<T>& rhs)
	{
		set<T>(rhs);

		return *this;
	}

	template <class U, std::enable_if<std::is_base_of<T, U>::value, int32>::type = 0>
	PWeakPtr<T>& operator=(PSharedPtr<U>&& rhs)
	{
		set<U>(rhs);

		return *this;
	}

	PWeakPtr<T>& operator=(PSharedPtr<T>&& rhs)
	{
		set<T>(rhs);

		return *this;
	}

	void Reset()
	{
		if (isValidPointer() == true)
		{
			subWeakCount();
			_ptr = nullptr;
			_pRefCount  = nullptr;
			_pWeakCount = nullptr;
		}
	}
	
	PSharedPtr<T> Pin() const
	{
		if (IsValid() == false)
		{
			return nullptr;
		}
		return GMemoryGlobalSystem::GetInstance().Wrap(_ptr);
	}

public:
	bool IsValid() const
	{
		if (_ptr == nullptr)
		{
			return false;
		}
		return _pRefCount->load() > 0;
	}

private:
	bool isValidPointer() const 
	{
		return _ptr != nullptr;
	}

	template <class U>
	void set(const PSharedPtr<U>& ptr)
	{
		if (ptr.IsValid() == false)
		{
			return;
		}

		Reset();

		PMemoryPrivate::PTemporaryOwner<U> owner(ptr);

		_ptr = owner.ptr;
		_pRefCount  = owner.pRefCount;
		_pWeakCount = owner.pWeakCount;

		addWeakCount();
	}

	template <class U>
	void copy(const PWeakPtr<U>& rhs)
	{
		if (rhs.IsValid() == false)
		{
			return;
		}

		Reset();

		PMemoryPrivate::PTemporaryOwner<U> owner(rhs);

		_ptr = owner.ptr;
		_pRefCount  = owner.pRefCount;
		_pWeakCount = owner.pWeakCount;

		addWeakCount();
	}

	template <class U>
	void move(PWeakPtr<U>&& rhs)
	{
		if (rhs.IsValid() == false)
		{
			return;
		}

		Reset();

		PMemoryPrivate::PTemporaryOwner<U> owner(std::move(rhs));

		_ptr = owner.ptr;
		_pRefCount  = owner.pRefCount;
		_pWeakCount = owner.pWeakCount;
	}

	void addWeakCount()
	{
		if (_pWeakCount == nullptr)
		{
			return;
		}

		_pWeakCount->fetch_add(1);
	}

	void subWeakCount()
	{
		if (_pWeakCount == nullptr)
		{
			return;
		}

		_pWeakCount->fetch_sub(1);
	}

};

class GMemoryGlobalSystem : public GGlobalSystemInstance<GMemoryGlobalSystem>
{
	struct HMemoryBlock
	{
		void*  Ptr  = nullptr;
		uint64 Size = 0;
		bool bIsClass = false;
		std::unique_ptr<HAtomicInt32> RefCount;
		std::unique_ptr<HAtomicInt32> WeakCount;
	};

	mutable HHashMap<const void*, HMemoryBlock> _allocatedMemoryBlocks;
	mutable HQueue<void*> _allocatedMemoryBlockQueue;

	mutable HMutex _mutex;
	int32 _processBlockCountPerFrame;

public:
	GMemoryGlobalSystem(int32 processBlockCountPerFrame = 10240);
	virtual ~GMemoryGlobalSystem();

protected:
	virtual void Update() override;

public:
	template<class T, class ...Args>
	PSharedPtr<T> Allocate(Args&& ... args) const
	{
		if (std::is_class<T>::value == true &&
			std::is_base_of<IMemoryObject, T>::value == false)
		{
			JG_ASSERT(false);
		}
		
		PSharedPtr<T> ptr;
		ptr._ptr = new T(args ...);

		HMemoryBlock memoryBlock;
		memoryBlock.Ptr = ptr._ptr;
		memoryBlock.Size	 = sizeof(T);
		memoryBlock.bIsClass = std::is_class<T>::value;
		memoryBlock.RefCount = std::make_unique<HAtomicInt32>();
		memoryBlock.WeakCount = std::make_unique<HAtomicInt32>();

		ptr._pWeakCount = memoryBlock.WeakCount.get();
		ptr._pRefCount = memoryBlock.RefCount.get();
		ptr._pRefCount->fetch_add(1);

		{
			HLockGuard<HMutex> lock(_mutex);
			_allocatedMemoryBlocks.emplace(ptr._ptr, std::move(memoryBlock));
			_allocatedMemoryBlockQueue.push(ptr._ptr);
		}

		IMemoryObject* memObject = ptr._ptr;
		memObject->Construct();

		return ptr;
	}

	template<class T>
	PSharedPtr<T> Wrap(const T* fromThis) const
	{
		if (std::is_class<T>::value == true &&
			std::is_base_of<IMemoryObject, T>::value == false)
		{
			JG_ASSERT(false);
		}

		if (fromThis == nullptr)
		{
			return PSharedPtr<T>();
		}

		HLockGuard<HMutex> lock(_mutex);
		if (_allocatedMemoryBlocks.find(fromThis) == _allocatedMemoryBlocks.end())
		{
			//unlock();
			return PSharedPtr<T>();
		}
		HMemoryBlock& memoryBlock = _allocatedMemoryBlocks[(const void*)fromThis];
		
		PSharedPtr<T> ptr;
		ptr._ptr = static_cast<T*>(memoryBlock.Ptr);
		ptr._pRefCount = memoryBlock.RefCount.get();
		ptr._pRefCount->fetch_add(1);

		return ptr;
	}

	template<class T, class U>
	PSharedPtr<T> RawFastCast(PSharedPtr<U> ptr)
	{
		if (std::is_base_of<T, U>::value == false && std::is_base_of<U, T>::value == false)
		{
			return nullptr;
		}

		return RawFastCastUnChecked<T, U>(ptr);
	}

	template<class T, class U>
	PWeakPtr<T> RawFastCast(PWeakPtr<U> ptr)
	{
		if (std::is_base_of<T, U>::value == false && std::is_base_of<U, T>::value == false)
		{
			return nullptr;
		}

		return RawFastCastUnChecked<T, U>(ptr);
	}

	template<class T, class U>
	PSharedPtr<T> RawDynamicCast(PSharedPtr<U> ptr)
	{
		if (std::is_base_of<T, U>::value == false && std::is_base_of<U, T>::value == false)
		{
			return nullptr;
		}

		return RawDynamicCastUnChecked<T, U>(ptr);
	}

	template<class T, class U>
	PWeakPtr<T> RawDynamicCast(PWeakPtr<U> ptr)
	{
		if (std::is_base_of<T, U>::value == false && std::is_base_of<U, T>::value == false)
		{
			return nullptr;
		}

		return RawDynamicCastUnChecked<T, U>(ptr);
	}

	template<class T, class U>
	PSharedPtr<T> RawFastCastUnChecked(PSharedPtr<U> ptr)
	{
		PSharedPtr<T> result;
		result._ptr = static_cast<T*>(ptr._ptr);
		result._pRefCount = ptr._pRefCount;
		result._pWeakCount = ptr._pWeakCount;
		result.addRefCount();

		return result;
	}

	template<class T, class U>
	PWeakPtr<T> RawFastCastUnChecked(PWeakPtr<U> ptr)
	{
		PWeakPtr<T> result;
		result._ptr = static_cast<T*>(ptr._ptr);
		result._pRefCount = ptr._pRefCount;
		result._pWeakCount = ptr._pWeakCount;
		result.addWeakCount();

		return result;
	}

	template<class T, class U>
	PWeakPtr<T> RawDynamicCastUnChecked(PWeakPtr<U> ptr)
	{
		T* result_ptr = dynamic_cast<T*>(ptr._ptr);
		if (result_ptr == nullptr)
		{
			return nullptr;
		}

		PWeakPtr<T> result;
		result._ptr = result_ptr;
		result._pRefCount  = ptr._pRefCount;
		result._pWeakCount = ptr._pWeakCount;
		result.addWeakCount();

		return result;
	}

	template<class T, class U>
	PSharedPtr<T> RawDynamicCastUnChecked(PSharedPtr<U> ptr)
	{
		T* result_ptr = dynamic_cast<T*>(ptr._ptr);
		if (result_ptr == nullptr)
		{
			return nullptr;
		}

		PSharedPtr<T> result;
		result._ptr = result_ptr;
		result._pRefCount = ptr._pRefCount;
		result._pWeakCount = ptr._pWeakCount;
		result.addRefCount();

		return result;
	}

	void Flush();
private:
	void garbageCollection(int32 level);
	int32 garbageCollectionInternal(int32 countPerFrame, bool bForce = false);
};


template<class T, class ...Args>
inline PSharedPtr<T> Allocate(Args ...args)
{
	return GMemoryGlobalSystem::GetInstance().Allocate<T>(args...);
}

template<class T>
inline PSharedPtr<T> Allocate(const T& data)
{
	PSharedPtr<T> result = GMemoryGlobalSystem::GetInstance().Allocate<T>();
	*result = data;
	return result;
}

template<class T>
inline PSharedPtr<T> SharedWrap(const T* fromThis)
{
	return GMemoryGlobalSystem::GetInstance().Wrap(fromThis);
}

template<class T, class U>
inline PSharedPtr<T> RawFastCast(PSharedPtr<U> ptr)
{
	return GMemoryGlobalSystem::GetInstance().RawFastCast<T, U>(ptr);
}

template<class T, class U>
inline T* RawFastCast(U* ptr)
{
	if (std::is_base_of<T, U>::value == false && std::is_base_of<U, T>::value == false)
	{
		return nullptr;
	}

	return static_cast<T*>(ptr);
}

template<class T, class U>
inline PWeakPtr<T> RawFastCast(PWeakPtr<U> ptr)
{
	return GMemoryGlobalSystem::GetInstance().RawFastCast<T, U>(ptr);
}

template<class T, class U>
inline PSharedPtr<T> RawDynamicCast(PSharedPtr<U> ptr)
{
	return GMemoryGlobalSystem::GetInstance().RawDynamicCast<T, U>(ptr);
}

template<class T, class U>
inline PWeakPtr<T> RawDynamicCast(PWeakPtr<U> ptr)
{
	return GMemoryGlobalSystem::GetInstance().RawDynamicCast<T, U>(ptr);
}

