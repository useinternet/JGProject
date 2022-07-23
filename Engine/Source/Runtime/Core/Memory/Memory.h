#pragma once

#include "CoreDefines.h"
#include "CoreSystem.h"

template<class T>
class PWeakPtr;

template<class T>
class PSharedPtr
{
private:
	friend class AMemoryGlobalSystem;
	friend PWeakPtr<T>;

	T* _ptr = nullptr;

	AtomicInt32* _pRefCount = nullptr;
	AtomicInt32* _pWeakCount = nullptr;
public:
	PSharedPtr()  = default;
	PSharedPtr(const PSharedPtr<T>& rhs)
	{
		copy(rhs);
	}
	PSharedPtr(PSharedPtr<T>&& rhs)
	{
		move(rhs);
	}

	~PSharedPtr()
	{
		if (IsValid() == false)
		{
			return;
		}

		_pRefCount->fetch_sub(1);
	}
public:
	PSharedPtr<T>& operator=(const PSharedPtr<T>& rhs)
	{
		copy(rhs);
		
		return *this;
	}
	PSharedPtr<T>& operator=(PSharedPtr<T>&& rhs)
	{
		move(rhs);

		return *this;
	}
	T* operator->()
	{
		return _ptr;
	}
	T& operator*()
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
private:
	void copy(const PSharedPtr<T>& rhs)
	{
		if (rhs.IsValid() == false)
		{
			return;
		}

		_ptr       = rhs._ptr;
		_pRefCount = rhs._pRefCount;
		_pWeakCount = rhs._pWeakCount;

		addRefCount();
	}
	void move(PSharedPtr<T>& rhs)
	{
		if (rhs.IsValid() == false)
		{
			return;
		}

		_ptr       = rhs._ptr;
		_pRefCount = rhs._pRefCount;
		_pWeakCount = rhs._pWeakCount;

		rhs._ptr = nullptr;
		rhs._pRefCount = nullptr;
		rhs._pWeakCount = nullptr;
	}
	void addRefCount()
	{
		if (_pRefCount == nullptr)
		{
			return;
		}

		_pRefCount->fetch_add(1);
	}
};


template<class T>
class PWeakPtr
{
	friend class AMemoryGlobalSystem;

	T* _ptr = nullptr;
	AtomicInt32* _pRefCount   = nullptr;
	AtomicInt32* _pWeakCount  = nullptr;
public:
	PWeakPtr() = default;
	PWeakPtr(const PWeakPtr<T>& rhs)
	{
		copy(rhs);
	}
	PWeakPtr(PWeakPtr<T>&& rhs)
	{
		move(rhs);
	}
	PWeakPtr(const PSharedPtr<T>& rhs)
	{
		set(rhs);
	}
	PWeakPtr(PSharedPtr<T>&& rhs)
	{
		set(rhs);
	}

	~PWeakPtr()
	{
		if (IsValid() == false)
		{
			return;
		}

		_pWeakCount->fetch_sub(1);
	}
public:
	PWeakPtr<T>& operator=(const PWeakPtr<T>& rhs)
	{
		copy(rhs);

		return *this;
	}
	PWeakPtr<T>& operator=(PWeakPtr<T>&& rhs)
	{
		move(rhs);

		return *this;
	}
	PWeakPtr<T>& operator=(const PSharedPtr<T>& rhs)
	{
		set(rhs);

		return *this;
	}
	PWeakPtr<T>& operator=(PSharedPtr<T>&& rhs)
	{
		set(rhs);

		return *this;
	}
	
	PSharedPtr<T> Pin() const
	{
		if (IsValid() == false)
		{
			return PSharedPtr<T>();
		}
		return AMemoryGlobalSystem::GetInstance().Wrap(_ptr);
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
	void set(const PSharedPtr<T>& ptr)
	{
		if (ptr.IsValid() == false)
		{
			return;
		}

		_ptr = ptr._ptr;
		_pRefCount  = ptr._pRefCount;
		_pWeakCount = ptr._pWeakCount;

		_pWeakCount->fetch_add(1);
	}
	void copy(const PWeakPtr<T>& rhs)
	{
		if (rhs.IsValid() == false)
		{
			return;
		}

		_ptr = rhs._ptr;
		_pRefCount  = rhs._pRefCount;
		_pWeakCount = rhs._pWeakCount;

		_pWeakCount->fetch_add(1);
	}
	void move(PWeakPtr<T>& rhs)
	{
		if (rhs.IsValid() == false)
		{
			return;
		}

		_ptr = rhs._ptr;
		_pRefCount = rhs._pRefCount;
		_pWeakCount = rhs._pWeakCount;

		rhs._ptr = nullptr;
		rhs._pRefCount = nullptr;
		rhs._pWeakCount = nullptr;
	}
};



class AMemoryGlobalSystem : public AGlobalSystemInstance <AMemoryGlobalSystem>
{
	struct PMemoryBlock
	{
		void*  Ptr  = nullptr;
		uint64 Size = 0;

		std::unique_ptr<AtomicInt32> RefCount;
		std::unique_ptr<AtomicInt32> WeakCount;
	};

	mutable PHashMap<void*, PMemoryBlock> _allocatedMemoryBlocks;
	mutable PQueue<void*> _allocatedMemoryBlockQueue;
	// Wrap
	// 메모리 시스템 특징
	// 메모리 할당 시 쓰레드 별로 메모리 수거
	//
	mutable AtomicBool _bLock = false;
	int32 _processBlockCountPerFrame;
public:
	AMemoryGlobalSystem(int32 processBlockCountPerFrame = 10);
	virtual ~AMemoryGlobalSystem();

protected:
	virtual void Update() override;

public:
	template<class T, class ...Args>
	PSharedPtr<T> Allocate(Args&& ... args) const
	{
		PSharedPtr<T> ptr;
		ptr._ptr = new T(args ...);

		PMemoryBlock memoryBlock;
		memoryBlock.Ptr = ptr._ptr;
		memoryBlock.Size	 = sizeof(T);
		memoryBlock.RefCount  = make_unique<AtomicInt32>();
		memoryBlock.WeakCount = make_unique<AtomicInt32>();

		ptr._pWeakCount = memoryBlock.WeakCount.get();
		ptr._pRefCount = memoryBlock.RefCount.get();
		ptr._pRefCount->fetch_add(1);

		waitAndLock();
		_allocatedMemoryBlocks.emplace(ptr._ptr, std::move(memoryBlock));
		_allocatedMemoryBlockQueue.push(ptr._ptr);
		unlock();
		return ptr;
	}

	template<class T>
	PSharedPtr<T> Wrap(T* fromThis) const
	{
		if (fromThis == nullptr)
		{
			return PSharedPtr<T>();
		}

		waitAndLock();
		if (_allocatedMemoryBlocks.find(fromThis) == _allocatedMemoryBlocks.end())
		{
			return PSharedPtr<T>();
		}
		PMemoryBlock& memoryBlock = _allocatedMemoryBlocks[fromThis];
		
		PSharedPtr<T> ptr;
		ptr._ptr = memoryBlock.Ptr;
		ptr._pRefCount = memoryBlock.RefCount.get();
		ptr._pRefCount->fetch_add(1);

		unlock();
		return ptr;
	}

	void Flush();
private:
	void waitAndLock() const;
	void unlock() const;
	void garbageCollection(int32 countPerFrame);
};


