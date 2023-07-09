#include "PCH/PCH.h"
#include "Memory.h"

GMemoryGlobalSystem::GMemoryGlobalSystem(int32 processBlockCountPerFrame)
{
	_processBlockCountPerFrame = processBlockCountPerFrame;
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

void GMemoryGlobalSystem::Flush()
{
	garbageCollection(-1);
}

void GMemoryGlobalSystem::garbageCollection(int32 level)
{
	HLockGuard<HMutex> lock(_mutex);

	if (level < 0)
	{
		while (_allocatedMemoryBlockQueue.empty() == false)
		{
			int32 deleteCount = garbageCollectionInternal((int32)_allocatedMemoryBlocks.size());

			if (deleteCount <= 0)
			{
				if (_allocatedMemoryBlocks.size() > 0)
				{
					garbageCollectionInternal((int32)_allocatedMemoryBlocks.size(), true);
				}
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

}

int32 GMemoryGlobalSystem::garbageCollectionInternal(int32 countPerFrame, bool bForce)
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

		const HMemoryBlock& memoryBlock = _allocatedMemoryBlocks[ptr];

		bool bIsClass   = memoryBlock.bIsClass;
		int32 refCount  = memoryBlock.RefCount->load();
		int32 weakCount = memoryBlock.WeakCount->load();
		_allocatedMemoryBlockQueue.pop();
		if (refCount == 0 || bForce)
		{
			_allocatedMemoryBlocks.erase(ptr);
			if (bIsClass == true)
			{
				((IMemoryObject*)ptr)->Destruction();
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
