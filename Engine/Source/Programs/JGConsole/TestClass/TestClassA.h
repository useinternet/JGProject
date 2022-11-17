#pragma once
#include "Core.h"

JGENUM(Config = JGConfig)
enum class ETestEnumA
{
	AAA, JGENUMMETA(Hidden, Category = Hidden)
	BBB, JGENUMMETA(Display, Category = Display)
	CCC, JGENUMMETA()
	DDD, JGENUMMETA(Hidden, Category = Hidden)
	EEE, JGENUMMETA(Display, Category = Display)
	MMM, JGENUMMETA(EXEC)
};

#include "Runtime/CodeGen/Module.JGConsole.TestClassA.generation.h"
JGCLASS(Category = TestClass, Config = JGConfig)
class JGTestClassA : public JGObject
{
	JG_GENERATED_CLASS_BODY
private:

	JGPROPERTY(Category = Setting, Meta = {Test1, Test2, Test3})
	int32 TestValue1 = 10;

	JGPROPERTY(Category = Setting, Meta = { Test4, Test5, Test6 })
	float32 TestValue2 = 2.0f;

	JGPROPERTY(Hidden, Category = { Setting, Hidden })
	PString TestValue3 = "Test";

public:
	JGFUNCTION(Category = Setting)
	float GetTestFunc()
	{
		return 1.0f;
	}

	JGFUNCTION(Hidden, Category = { Setting, Hidden })
	void TestFuncFunc(float v, float a, int32 c)
	{
		int n = 0;
	}

	JGFUNCTION(exec, Category = { Setting, Hidden })
	virtual void ExeFunc()
	{

	}
};




