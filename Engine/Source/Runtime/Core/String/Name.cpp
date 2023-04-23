#include "PCH/PCH.h"
#include "Name.h"
#include "String/String.h"
#include "StringTable.h"

PName::PName() 
{
	reset();
}

PName::PName(uint64 id)
{
	set(id);
}

PName::PName(const PString& str)
{
	set(str);
}

PName::PName(const PName& name)
{
	copy(name);
}

PName::PName(PName&& name)
{
	move(std::move(name));
}

PName::~PName()
{
	reset();
}

PName& PName::operator=(uint64 id)
{
	set(id);

	return *this;
}

PName& PName::operator=(const PString& str)
{
	set(str);

	return *this;
}

PName& PName::operator=(const PName& name)
{
	copy(name);

	return *this;
}

PName& PName::operator=(PName&& name)
{
	move(std::move(name));

	return *this;
}

bool PName::operator==(uint64 id) const
{
	return _id == id;
}

bool PName::operator!=(uint64 id) const
{
	return _id != id;
}

bool PName::operator==(const PName& name) const
{
	return _id == name._id;
}

bool PName::operator!=(const PName& name) const
{
	return _id != name._id;
}

const uint64& PName::GetID() const
{
	return _id;
}

void PName::ToString(PString& outStr) const
{
	GStringTable::GetInstance().FindString(_id, &outStr);
}

PString PName::ToString() const
{
	PString result;
	ToString(result);
	return result;
}

void PName::set(uint64 id)
{
	reset();

	if (id != NAME_NONE)
	{
		PString str;
		GStringTable::GetInstance().FindString(id, &str);

		set(str);
	}
}

void PName::set(const PString& str)
{
	reset();

	GStringTable::GetInstance().RegisterString(str, &_id, &_weakRefCount);
#ifdef _DEBUG
	_str = str.GetRawString();
#endif
	addRefCount();
}

void PName::copy(const PName& name)
{
	reset();

	_id = name._id;
	_weakRefCount = name._weakRefCount;
#ifdef _DEBUG
	_str = name._str;
#endif
	addRefCount();
}

void PName::move(PName&& name)
{
	reset();

	_id        = name._id;
	_weakRefCount = name._weakRefCount;
#ifdef _DEBUG
	_str = std::move(name._str);
#endif
	name._id = NAME_NONE;
	name._weakRefCount.reset();
}

void PName::reset()
{
	subRefCount();

	_id = NAME_NONE;
	_weakRefCount.reset();
}

void PName::addRefCount()
{
	if (_weakRefCount.expired())
	{
		_id = NAME_NONE;
		_weakRefCount.reset();
	}
	else
	{
		_weakRefCount.lock()->fetch_add(1);
	}
}

void PName::subRefCount()
{
	if (_weakRefCount.expired())
	{
		_id = NAME_NONE;
		_weakRefCount.reset();
	}
	else
	{
		_weakRefCount.lock()->fetch_sub(1);
	}
}
