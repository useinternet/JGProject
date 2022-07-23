#include "Memory.h"



AMemoryGlobalSystem::AMemoryGlobalSystem(int32 processBlockCountPerFrame)
{
	_processBlockCountPerFrame = processBlockCountPerFrame;
	_bLock = false;
}
AMemoryGlobalSystem::~AMemoryGlobalSystem()
{

	while (_allocatedMemoryBlockQueue.empty())
	{
		void* ptr = _allocatedMemoryBlockQueue.front();

		delete ptr;
		ptr = nullptr;

		_allocatedMemoryBlockQueue.pop();
	}
	_allocatedMemoryBlocks.clear();
}

void AMemoryGlobalSystem::Update()
{
	garbageCollection(_processBlockCountPerFrame);
}
void AMemoryGlobalSystem::waitAndLock() const
{
	while (_bLock) {}
	_bLock = true;
}
void AMemoryGlobalSystem::unlock() const
{
	_bLock = false;
}

void AMemoryGlobalSystem::Flush()
{
	garbageCollection(-1);
}

void AMemoryGlobalSystem::garbageCollection(int32 countPerFrame)
{
	waitAndLock();

	int32 tempCnt = 0;

	while (_allocatedMemoryBlockQueue.empty() == false)
	{
		if (tempCnt > countPerFrame && countPerFrame >= 0)
		{
			break;
		}
		++tempCnt;

		void* ptr = _allocatedMemoryBlockQueue.front();

		_allocatedMemoryBlockQueue.pop();

		const PMemoryBlock& memoryBlock = _allocatedMemoryBlocks[ptr];

		int32 refCount = memoryBlock.RefCount->load();
		int32 weakCount = memoryBlock.WeakCount->load();

		if (refCount == 0 && weakCount == 0)
		{
			_allocatedMemoryBlocks.erase(ptr);

			delete ptr;
			ptr = nullptr;
		}
		else if(countPerFrame >= 0)
		{
			_allocatedMemoryBlockQueue.push(ptr);
		}
	}

	unlock();
}
