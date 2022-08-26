#pragma once
#include "Core.h"



JGSTRUCT()
// Auto Code Gen
struct PTestObject;
PSharedPtr<JGStruct> Module_Core_Code_Generation_Create_JGStruct_TestObject(const PTestObject* fromThis);
//

struct PTestObject : public IMemoryObject
{
	GENERATED_JGCLASS_BODY()

	// Auto Code Gen
	PSharedPtr<JGStruct> GetStruct() const
	{
		return Module_Core_Code_Generation_Create_JGStruct_TestObject(this);
	}

	static PSharedPtr<JGStruct> GetStaticStruct()
	{
		return GObjectGlobalSystem::GetInstance().GetStaticStruct<PTestObject>();
	}

	//
	JGPROPERTY()
	int TestValue1 = -1;

	JGPROPERTY()
	int32 TestValue3 = 3;

	JGPROPERTY(TestMeta, Category = TestCategory, BlueprintOnly)
	int64 TestValue2 = 1;

	JGFUNCTION()
	void TestFunc();

	JGFUNCTION(TestMetaFunc)
	int32 TestFunc2(float32 value, int32 value2);

	JGFUNCTION()
	float32 TestFunc3(const PString& str);
};