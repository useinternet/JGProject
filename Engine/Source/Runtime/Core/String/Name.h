#pragma once


/*
String TableÀÌ Á¸Àç
std::unormed_map ->


*/
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include <iostream>


#pragma warning(disable : 4996)

class PString;
class PRawString;
class PRawWString;

class PName : public IMemoryObject
{
	uint64 _id = NAME_NONE;

	AtomicInt32* _pRefCount = nullptr;
public:
	PName();
	PName(uint64 id);
	PName(const PString& str);
	PName(const PName& name);

	PName(PName&& name);

	virtual ~PName();
public:
	PName& operator=(uint64 id);
	PName& operator=(const PString& str);
	PName& operator=(const PName& name);

	PName& operator=(PName&& name);

	bool operator==(uint64 id) const;
	bool operator!=(uint64 id) const;
	bool operator==(const PName& name) const;
	bool operator!=(const PName& name) const;

public:
	const uint64& GetID() const;
	const PString&     ToString() const;
	const PRawString&  ToRawString() const;
	const PRawWString& ToRawWString() const;

private:
	void set(const PString& str);
	void copy(const PName& name);
	void move(PName&& name);
	void reset();
	void addRefCount();
	void subRefCount();
};

namespace std {
	template <>
	struct hash<PName>
	{
		std::size_t operator()(const PName& k) const noexcept
		{
			return k.GetID();
		}
	};
}