#include "Memory.h"



AMemoryGlobalSystem::AMemoryGlobalSystem(uint32 processBlockCountPerFrame)
{
	PList<ThreadID> threadIDs = ACoreSystem::GetAllThreadIDs();
	for (ThreadID id : threadIDs)
	{
		_reservedJobMap.emplace(id, PReservedJobs());
	}

	_processBlockCountPerFrame = processBlockCountPerFrame;
	_bLock = false;
}

void AMemoryGlobalSystem::Update()
{
	lock();

	// add
	uint32 tempCnt = 0;

	for (PPair<const ThreadID, PReservedJobs>& pair : _reservedJobMap)
	{
		PReservedJobs& reservedJob = pair.second;

		while (reservedJob.AddedMemoryBlocks.empty() == false)
		{
			addMemoryBlock(reservedJob.AddedMemoryBlocks.front());
			reservedJob.AddedMemoryBlocks.pop();

			tempCnt += 1;
			if (tempCnt > _processBlockCountPerFrame)
			{
				break;
			}
		}

		if (tempCnt > _processBlockCountPerFrame)
		{
			break;
		}
	}

	tempCnt = 0;

	for (PPair<const ThreadID, PReservedJobs>& pair : _reservedJobMap)
	{
		PReservedJobs& reservedJob = pair.second;

		while (reservedJob.RemovedMemoryBlocks.empty() == false)
		{
			removeMemoryBlock(reservedJob.RemovedMemoryBlocks.front());
			reservedJob.RemovedMemoryBlocks.pop();

			tempCnt += 1;
			if (tempCnt > _processBlockCountPerFrame)
			{
				break;
			}
		}

		if (tempCnt > _processBlockCountPerFrame)
		{
			break;
		}
	}

	unlock();
}

void AMemoryGlobalSystem::deallocate(void* ptr)
{
	ThreadID currentThreadID = std::this_thread::get_id();
	PReservedJobs& reservedJob = _reservedJobMap[currentThreadID];

	PMemoryBlock memoryBlock;
	memoryBlock.Ptr = ptr;

	wait();

	reservedJob.RemovedMemoryBlocks.push(memoryBlock);
}

void AMemoryGlobalSystem::wait()
{
	while (_bLock) {}
}
void AMemoryGlobalSystem::lock()
{
	
	PLockGuard<PMutex> lock(_lockMutex);
	if (_bLock == false)
	{
		_bLock = true;
	}
}
void AMemoryGlobalSystem::unlock()
{
	PLockGuard<PMutex> lock(_lockMutex);
	if (_bLock == true)
	{
		_bLock = false;
	}
}

void AMemoryGlobalSystem::addMemoryBlock(const PMemoryBlock& memoryBlock)
{
	if (memoryBlock.Ptr == nullptr)
	{
		return;
	}

	PMemoryBlock& originMemoryBlock = _allocatedMemoryBlockMap[memoryBlock.Ptr];
	originMemoryBlock.RefCount += 1;
	originMemoryBlock.Size = memoryBlock.Size;
}

void AMemoryGlobalSystem::removeMemoryBlock(const PMemoryBlock& memoryBlock)
{
	if (memoryBlock.Ptr == nullptr)
	{
		return;
	}
	PMemoryBlock& originMemoryBlock = _allocatedMemoryBlockMap[memoryBlock.Ptr];
	originMemoryBlock.RefCount -= 1;

	if (originMemoryBlock.RefCount == 0)
	{
		void* deletePtr = originMemoryBlock.Ptr;

		_allocatedMemoryBlockMap.erase(deletePtr);

		delete deletePtr;
		deletePtr = nullptr;
	}
}
