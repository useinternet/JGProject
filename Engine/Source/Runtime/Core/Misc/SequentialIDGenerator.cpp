#include "PCH/PCH.h"
#include "SequentialIDGenerator.h"

PSequentialIDGenerator::PSequentialIDGenerator()
{
	Reset();
}

uint64 PSequentialIDGenerator::NewID()
{
	uint64 ID = 0;

	if (_pendingIDQueue.empty() == false)
	{
		ID = _pendingIDQueue.front(); _pendingIDQueue.pop();
	}
	else
	{
		ID = ++_idOffset;
	}

	_idPool.insert(ID);

	return ID;
}

bool PSequentialIDGenerator::RetrieveID(uint64 id)
{
	if (_idPool.find(id) == _idPool.end())
	{
		return false;
	}

	_pendingIDQueue.push(id);

	return true;
}

void PSequentialIDGenerator::Reset()
{
	_idOffset = 0;
	_idPool.clear();
	_pendingIDQueue = HQueue<uint64>();
}
