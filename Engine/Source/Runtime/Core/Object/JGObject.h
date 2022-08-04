#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "String/String.h"


// �̰Ŵ� object�� destroy�� ���� �� ������ �������
template< class T>
class PWeakJGObjectPtr
{

};

// ���
template< class T>
class PJGObjectPtr
{

};



class JGObject : public IMemoryObject
{
	// JGObject �� ���Ե� ������


	/* ��ü ���� ����
	-- Class --
		+ VTable : ��� ����
		+ Field  : ����, �Լ� ����, ������


	-- Struct --
		+ Field  : ����, �Լ� ����, ������

	-- Interface --
		+ Interface ��� �ߺ��� ������ ����
		+ Field  : ���� �Լ� ����, ������



	-- Enum --
		

	-- EnumFlags --

	
	*/
	/*
	JGClass* GetClass();
	JGClass* GetStaticClass();
	*/

private:
	PString _name;
public:
	virtual ~JGObject() = default;

public:
	void SetName(const PString& name);

	const PString& GetName() const;
};

