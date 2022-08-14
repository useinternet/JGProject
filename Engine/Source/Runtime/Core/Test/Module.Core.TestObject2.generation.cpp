#include "TestObject2.h"


PSharedPtr<JGClass> Module_Core_Code_Generation_Create_Static_JGClass_TestClassA();
PSharedPtr<JGClass> Module_Core_Code_Generation_Create_Static_JGClass_TestClassA()
{
	PList<PSharedPtr<JGProperty>> PropertyMap;

	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(
		JGTYPE(int32),
		"TestValue1"));

	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(
		JGTYPE(int64),
		"TestValue2",
		PObjectGlobalsPrivateUtils::MakeStaticMeta(
			{
				PPair<PString, PString>("TestMeta", ""),
				PPair<PString, PString>("Category", "TestCategory"),
				PPair<PString, PString>("BlueprintOnly", "")
			})));

	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(
		JGTYPE(int32),
		"TestValue3"));

	PList<PSharedPtr<JGFunction>> FunctionMap;

	FunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction(
		"TestFunc",
		nullptr));

	FunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction(
		"TestFunc2",
		PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int32), ""),
		{
			PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(float32), "value"),
			PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int32), "value2"),
		},
		PObjectGlobalsPrivateUtils::MakeStaticMeta({
			PPair<PString, PString>("TestMetaFunc", "")
			})));

	FunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction(
		"TestFunc3",
		PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(float32), ""),
		{
			PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(PString), "str")
		}));

	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGTYPE(PTestObject), PropertyMap, FunctionMap);
}

PSharedPtr<JGClass> Module_Core_Code_Generation_Create_JGClass_TestClassA(const JGTestClassA* fromThis);
{
	return nullptr;
}