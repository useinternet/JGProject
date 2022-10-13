#pragma once

#include "Core.h"



class JGTestClassA;
PSharedPtr<JGClass> Module_Core_Code_Generation_Create_JGClass_TestClassA(const JGTestClassA* fromThis);
JGCLASS()
class JGTestClassA : public JGObject
{
	JG_GENERATED_CLASS_BODY()
	PSharedPtr<JGStruct> GetStruct() const
	{
		return Module_Core_Code_Generation_Create_JGClass_TestClassA(this);
	}

	static PSharedPtr<JGStruct> GetStaticStruct()
	{
		return GObjectGlobalSystem::GetInstance().GetStaticStruct<JGTestClassA>();
	}



	//
	JGPROPERTY()
	int TestValue1 = -1;

	JGPROPERTY()
	int32 TestValue3 = 3;

	JGPROPERTY(TestMeta, Category = { TestCategory, BlueprintOnly })
	int64 TestValue2 = 1;

	JGFUNCTION()
	virtual void TestFunc()
	{

	}

	JGFUNCTION(TestMetaFunc)
	virtual int32 TestFunc2(float32 value, int32 value2)
	{

	}

	JGFUNCTION()
	float32 TestFunc3(const PString& str)
	{

	}
};