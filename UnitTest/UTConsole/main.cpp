#include <iostream>
#include <unordered_map>
#include <crtdbg.h>
using namespace std;


class A
{
	int* n = nullptr;
public:
	A()
	{
		n = new int;
		*n = 5;
		printf("%d \n", *n);
	}
	A(const A& a)
	{
		n = a.n;
	}
	A& operator=(const A& a)
	{
		n = a.n;
		return *this;
	}
	~A()
	{
		printf("%d ", (*n) * 2);
		delete n;
		n = nullptr;
	}
};
int main()
{
	A a;

	A b = a;

}