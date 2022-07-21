#pragma once

#include "CoreDefines.h"
#include "CoreSystem.h"


template<class T>
class PSharedPtr
{
	friend class AMemoryGlobalSystem;

	T* ptr = nullptr;
public:
	PSharedPtr()  = default;
	~PSharedPtr() = default;
public:
	


};


template<class T>
class PWeakPtr
{



};



class AMemoryGlobalSystem : public AGlobalSystemInstance <AMemoryGlobalSystem>
{
	struct PMemoryBlock
	{
		void*  Ptr  = nullptr;
		uint64 Size = 0;
		uint32 RefCount  = 0;
		uint32 WeakCount = 0;
	};

	// Thread�� Task ���
	// �޸� ����
	// �޸� �߰�
	// �޸� Wrap
	struct PReservedJobs
	{
		PQueue<PMemoryBlock> AddedMemoryBlocks;
		PQueue<PMemoryBlock> RemovedMemoryBlocks;
	};

	PHashMap<void*, PMemoryBlock>     _allocatedMemoryBlockMap;


	mutable PHashMap<ThreadID, PReservedJobs> _reservedJobMap;

	// Wrap
	// �޸� �ý��� Ư¡
	// �޸� �Ҵ� �� ������ ���� �޸� ����
	//
	bool   _bLock = false;
	PMutex _lockMutex;

	uint32 _processBlockCountPerFrame;
public:
	AMemoryGlobalSystem(uint32 processBlockCountPerFrame = 10);
	virtual ~AMemoryGlobalSystem() = default;

protected:
	virtual void Update() override;
public:
	template<class T, class ...Args>
	PSharedPtr<T> Allocate(Args&& ... args) const
	{
		PSharedPtr<T> ptr;
		ptr.ptr		 = new T(args ...);

		ThreadID currentThreadID   = std::this_thread::get_id();
		PReservedJobs& reservedJob = _reservedJobMap[currentThreadID];

		PMemoryBlock memoryBlock;
		memoryBlock.Ptr		 = ptr.ptr;
		memoryBlock.Size	 = sizeof(T);

		wait();

		reservedJob.AddedMemoryBlocks.push(memoryBlock);

		return ptr;
	}

	template<class T>
	PSharedPtr<T> Wrap(T* fromThis) const
	{
		ThreadID currentThreadID = std::this_thread::get_id();
		PReservedJobs& reservedJob = _reservedJobMap[currentThreadID];

		PMemoryBlock memoryBlock;
		memoryBlock.Ptr  = fromThis;
		memoryBlock.Size = sizeof(T);

		wait();

		reservedJob.AddedMemoryBlocks.push(memoryBlock);
	}
private:
	void deallocate(void* ptr);
	void wait();
	void lock();
	void unlock();
	void addMemoryBlock(const PMemoryBlock& memoryBlock);
	void removeMemoryBlock(const PMemoryBlock& memoryBlock);
};


