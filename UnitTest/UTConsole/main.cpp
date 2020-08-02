#include <iostream>
#include <unordered_map>
#include <crtdbg.h>
using namespace std;


class A
{
public:
	virtual void print() const
	{
		cout << "A" << endl;
	}
};

class B : public A
{
public:
	virtual void print() const
	{
		cout << "B" << endl;
	}
};

class C : public A
{
public:
	virtual void print() const
	{
		cout << "C" << endl;
	}
};


void sampleFunc(const A& a)
{
	a.print();
}
int main()
{
	C c;
	sampleFunc(c);

}