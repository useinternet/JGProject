#include "TestObject.h"

void PTestObject::TestFunc()
{
	printf("TestObject::TestFunc");
}

int32 PTestObject::TestFunc2(float32 value, int32 value1)
{
	printf("TestObject::TestFunc2 %f", value);
	return 1;
}

float32 PTestObject::TestFunc3(const PString& str)
{
	printf("TestObject::TestFunc3 %s", str.GetCStr());
	return 1;
}
