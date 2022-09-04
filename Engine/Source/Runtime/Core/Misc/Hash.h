#pragma once




#include "CoreDefines.h"


class PHash : public IMemoryObject
{
public:
	PHash() = default;
	virtual ~PHash() = default;
public:
	template<class T>
	static uint64 Hash(const T& obj)
	{
		return (uint64)(std::hash<T>()(obj));
	}
};