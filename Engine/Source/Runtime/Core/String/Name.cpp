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
	// // O: ���⿡ return ���� �����մϴ�.
}

PName& PName::operator=(const PString& str)
{
	// // O: ���⿡ return ���� �����մϴ�.
}

PName& PName::operator=(const PName& name)
{
	// // O: ���⿡ return ���� �����մϴ�.
}

PName& PName::operator=(PName&& name)
{
	// // O: ���⿡ return ���� �����մϴ�.
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
	// // O: ���⿡ return ���� �����մϴ�.
}

const PString& PName::ToString() const
{
	// // O: ���⿡ return ���� �����մϴ�.
}

const PRawString& PName::ToRawString() const
{
	// // O: ���⿡ return ���� �����մϴ�.
}

const PRawWString& PName::ToRawWString() const
{
	// // O: ���⿡ return ���� �����մϴ�.
}
