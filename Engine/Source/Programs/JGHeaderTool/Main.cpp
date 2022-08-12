

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

class D : public IMemoryObject
{
	int Value3 = 2;
public:
	virtual ~D() = default;
};

int main()
{

	GCoreSystem::Create();

	{
		// 
		PSharedPtr<JGStruct> staticStruct = PTestObject::GetStaticStruct();

		PTestObject testObject;
		PSharedPtr<JGStruct> Struct = testObject.GetStruct();


		
	}
	GCoreSystem::Update();

	

	GCoreSystem::Destroy();


	return 0;
}