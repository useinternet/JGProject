#pragma once
#include "Core.h"



JGENUM(TestOnlyDisplay, BlueprintOnly)
enum class ETestEnum
{
	_1, JGENUMMETA(Display = {DIOSK}, TypeTest)
	_2, JGENUMMETA(Display = {aOSIDJF}, TypeTest)
};


// Auto Code Gen
class PTestObject;
PSharedPtr<JGStruct> Module_Core_Code_Generation_Create_JGStruct_TestObject(const PTestObject* fromThis);
//
JGCLASS(TEST, Category = {asoijfd,aosdifjaosdf})
class PTestObject : public JGObject
{
	JG_GENERATED_CLASS_BODY()

	// Auto Code Gen
	PSharedPtr<JGStruct> GetStruct() const
	{
		return Module_Core_Code_Generation_Create_JGStruct_TestObject(this);
	}

	static PSharedPtr<JGStruct> GetStaticStruct()
	{
		return GObjectGlobalSystem::GetInstance().GetStaticStruct<PTestObject>();
	}
public:
	//
	JGPROPERTY()
	int TestValue1 = -1;

	JGPROPERTY()
	int32 TestValue3 = 3;

	JGPROPERTY(TestMeta, Category = { TestCategory, BlueprintOnly })
	int64 TestValue2 = 1;

	JGFUNCTION()
	void TestFunc();

	JGFUNCTION(TestMetaFunc)
	int32 TestFunc2(float32 value, int32 value2);

	JGFUNCTION()
	float32 TestFunc3(const PString& str);
};