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
	// JGObject 에 포함될 정보들


	/* 객체 정보 저장
	-- Class --
		+ VTable : 상속 정보
		+ Field  : 변수, 함수 정보, 사이즈


	-- Struct --
		+ Field  : 변수, 함수 정보, 사이즈

	-- Interface --
		+ Interface 상속 중복시 컴파일 에러
		+ Field  : 변수 함수 정보, 사이즈



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

