#pragma once

#include "Core.h"

class PTestObject;

JGSTRUCT()
struct PTestObject2
{
	JGPROPERTY()
	int TestValue;

	JGPROPERTY()
	float TestValue2;

	JGPROPERTY()
	PSharedPtr<PTestObject> TestObject;

};