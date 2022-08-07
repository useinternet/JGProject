#include "Name.h"
#include "String/String.h"
#include "StringTable.h"

PName::PName() {}
PName::PName(uint64 id)
{
}
PName::PName(const PString& str)
{
	GStringTable::GetInstance().RegisterString(str, &_id, &_pRefCount);
}

PName::PName(const PName& name)
{
}

PName::PName(PName&& name)
{
}

PName::~PName()
{
}

PName& PName::operator=(uint64 id)
{
	// // O: 여기에 return 문을 삽입합니다.
}

PName& PName::operator=(const PString& str)
{
	// // O: 여기에 return 문을 삽입합니다.
}

PName& PName::operator=(const PName& name)
{
	// // O: 여기에 return 문을 삽입합니다.
}

PName& PName::operator=(PName&& name)
{
	// // O: 여기에 return 문을 삽입합니다.
}

bool PName::operator==(uint64 id) const
{
	return false;
}

bool PName::operator!=(uint64 id) const
{
	return false;
}

bool PName::operator==(const PName& name) const
{
	return false;
}

bool PName::operator!=(const PName& name) const
{
	return false;
}

const uint64& PName::GetID() const
{
	// // O: 여기에 return 문을 삽입합니다.
}

const PString& PName::ToString() const
{
	// // O: 여기에 return 문을 삽입합니다.
}

const PRawString& PName::ToRawString() const
{
	// // O: 여기에 return 문을 삽입합니다.
}

const PRawWString& PName::ToRawWString() const
{
	// // O: 여기에 return 문을 삽입합니다.
}
