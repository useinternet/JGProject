#pragma once

#include "CoreDefines.h"
#include "Memory/Memory.h"
#include <iostream>


#pragma warning(disable : 4996)

class PString;

class PName : public IMemoryObject
{
	uint64 _id = NAME_NONE;

	std::weak_ptr<HAtomicInt32> _weakRefCount;
#ifdef _DEBUG
	HRawString _str;
#endif
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
	void    ToString(PString& outStr) const;
	PString ToString() const;
private:
	void set(uint64 id);
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