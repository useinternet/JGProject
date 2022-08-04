#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "String/String.h"


// 이거는 object가 destroy에 진입 전 연결을 끊어버림
template< class T>
class PWeakJGObjectPtr
{

};

// 평범
template< class T>
class PJGObjectPtr
{

};



class JGObject : public IMemoryObject
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
	PString _name;
public:
	virtual ~JGObject() = default;

public:
	void SetName(const PString& name);

	const PString& GetName() const;
};

