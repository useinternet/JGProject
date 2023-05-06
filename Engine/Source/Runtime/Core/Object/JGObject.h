#pragma once
#include "CoreDefines.h"
#include "ObjectDefines.h"
#include "FileIO/Jsonable.h"
#include "String/Name.h"
#include "Memory/Memory.h"
#include "Misc/Delegate.h"

class JGType;
class JGObject : public IMemoryObject, public IJsonable
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

protected:
	virtual void OnWriteJson(PJsonData& json) const {}
	virtual void OnReadJson(const PJsonData& json) {}

	virtual void WriteJson(PJsonData& json) const override;
	virtual void ReadJson(const PJsonData& json) override;
};

