#pragma once
#include "CoreDefines.h"
#include "ObjectDefines.h"
#include "Memory/Memory.h"
#include "String/String.h"
#include "String/Name.h"

class JGType;
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
	mutable PName _name;
public:
	virtual ~JGObject() = default;

public:
	virtual JGType GetType() const;

	void SetName(const PName& name);

	const PName& GetName() const;
};

