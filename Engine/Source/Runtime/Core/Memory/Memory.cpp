#include "Memory.h"

GMemoryGlobalSystem::GMemoryGlobalSystem(int32 processBlockCountPerFrame)
{
	_processBlockCountPerFrame = processBlockCountPerFrame;
	_bLock = false;
}

GMemoryGlobalSystem::~GMemoryGlobalSystem()
{
	Flush();
	_allocatedMemoryBlocks.clear();
}

void GMemoryGlobalSystem::Update()
{
	garbageCollection(1);
}
void GMemoryGlobalSystem::waitAndLock() const
{
	while (_bLock) {}
	_bLock = true;
}
void GMemoryGlobalSystem::unlock() const
{
	_bLock = false;
}

void GMemoryGlobalSystem::Flush()
{
	garbageCollection(-1);
}

void GMemoryGlobalSystem::garbageCollection(int32 level)
{
	waitAndLock();

	if (level < 0)
	{
		while (_allocatedMemoryBlockQueue.empty() == false)
		{
			int32 deleteCount = garbageCollectionInternal((int32)_allocatedMemoryBlocks.size());

			if (deleteCount <= 0)
			{
				break;
			}
		}
	}
	else
	{
		int32 tempCnt = 0;

		while (_allocatedMemoryBlockQueue.empty() == false && tempCnt <= level)
		{
			++tempCnt;
			garbageCollectionInternal(_processBlockCountPerFrame);
		}
	}

	unlock();
}

int32 GMemoryGlobalSystem::garbageCollectionInternal(int32 countPerFrame)
{
	int32 deleteCount = 0;
	int32 tempCnt     = 0;

	while (_allocatedMemoryBlockQueue.empty() == false)
	{
		if (tempCnt > countPerFrame)
		{
			break;
		}
		++tempCnt;

		void* ptr = _allocatedMemoryBlockQueue.front();

		_allocatedMemoryBlockQueue.pop();

		const PMemoryBlock& memoryBlock = _allocatedMemoryBlocks[ptr];

		bool bIsClass   = memoryBlock.bIsClass;
		int32 refCount  = memoryBlock.RefCount->load();
		int32 weakCount = memoryBlock.WeakCount->load();

		if (refCount == 0)
		{
			_allocatedMemoryBlocks.erase(ptr);
			if (bIsClass == true)
			{
				delete (IMemoryObject*)ptr;
			}
			else
			{
				delete ptr;
			}

			ptr = nullptr;

			++deleteCount;
		}
		else
		{
			_allocatedMemoryBlockQueue.push(ptr);
		}
	}

	return deleteCount;
}
