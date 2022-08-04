

//
#include "Core.h"
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
		PSharedPtr<A> A2 = Allocate<B>();
		PSharedPtr<D> D2;
		PSharedPtr<C> C2 = Allocate<C>();

		A2 = C2;

		//A2 = D2;
	
		PSharedPtr<A> AAA;

		AAA = std::move(A2);
			

	}


	GCoreSystem::Destroy();
	return 0;
}