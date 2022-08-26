

//
#include "Core.h"
#include "String/StringTable.h"
#include "Test/TestObject.h"
#include <iostream>





class A : public IMemoryObject
{
	int Value = -1;
public:
	virtual ~A() = default;
};


class B : public A
{
	int Value2 = 3;
public:
	virtual ~B() = default;
};


class C : public B
{
	int Value3 = 2;
public:
	virtual ~C() = default;
};

class D : public IMemoryObject, public IJsonable
{
	int Value3 = 2;
	PString TestValue = "TestVa;ie";
public:
	virtual ~D() = default;

protected:
	virtual void WriteJson(PJsonData& json) const override
	{
		json.AddMember(Value3);
		json.AddMember(TestValue);
	}
	virtual void ReadJson(PJsonData& json) override
	{
		PJsonData val;
		if (json.FindMemberFromIndex(0, &val) == true)
		{
			val.GetData(&Value3);
		}
		
		if (json.FindMemberFromIndex(1, &val) == true)
		{
			val.GetData(&TestValue);
		}
	}
};

// 생성과 동시에 할당하는 로직 고민

int main()
{




	return 0;
}