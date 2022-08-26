#include "TestObject.h"



// JGStruct

PSharedPtr<JGStruct> Module_Core_Code_Generation_Create_Static_JGStruct_TestObject();

PSharedPtr<JGStruct> Module_Core_Code_Generation_Create_Static_JGStruct_TestObject()
{
	PList<PSharedPtr<JGProperty>> PropertyMap;

	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(
		JGTYPE(int32),
	"TestValue1"));

	PropertyMap.push_back (PObjectGlobalsPrivateUtils::MakeStaticProperty(
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
		PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(void), "")));

	FunctionMap.push_back (PObjectGlobalsPrivateUtils::MakeStaticFunction(
		"TestFunc2",
		PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int32), ""),
		{
			PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(float32), "value"),
			PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int32), "value2"),
		}, 
		PObjectGlobalsPrivateUtils::MakeStaticMeta({
			PPair<PString, PString>("TestMetaFunc", "")
		})));

	FunctionMap.push_back (PObjectGlobalsPrivateUtils::MakeStaticFunction(
		"TestFunc3",
		PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(float32), ""),
		{
			PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(PString), "str")
		}));

	return PObjectGlobalsPrivateUtils::MakeStaticStruct(JGTYPE(PTestObject), PropertyMap, FunctionMap);
}

PSharedPtr<JGStruct> Module_Core_Code_Generation_Create_JGStruct_TestObject(const PTestObject* fromThis)
{
	PTestObject* noneConstThisPtr = const_cast<PTestObject*>(fromThis);

	PSharedPtr<JGStruct> Struct =  PObjectGlobalsPrivateUtils::MakeStruct(fromThis, GObjectGlobalSystem::GetInstance().GetStaticStruct(JGTYPEID(PTestObject)));

	if (Struct->HasFunction(PName("TestFunc")) == true)
	{
		std::function<void()> functionRef = std::bind(&PTestObject::TestFunc, noneConstThisPtr);

		if (PObjectGlobalsPrivateUtils::BindFunction(fromThis, Struct->FindFunction(PName("TestFunc")), functionRef) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "Fail Bind Function: TestFunc");
		}
	}

	if (Struct->HasFunction(PName("TestFunc2")) == true)
	{
		std::function<int32(float32, int32)> functionRef = std::bind(&PTestObject::TestFunc2, noneConstThisPtr, std::placeholders::_1, std::placeholders::_2);

		if (PObjectGlobalsPrivateUtils::BindFunction(fromThis, Struct->FindFunction(PName("TestFunc2")), functionRef) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "Fail Bind Function: TestFunc2");
		}
	}

	if (Struct->HasFunction(PName("TestFunc3")) == true)
	{
		std::function<float32(const PString&)> functionRef = std::bind(&PTestObject::TestFunc3, noneConstThisPtr, std::placeholders::_1);

		if (PObjectGlobalsPrivateUtils::BindFunction(fromThis, Struct->FindFunction(PName("TestFunc3")), functionRef) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "Fail Bind Function: TestFunc3");
		}
	}

	return Struct;
}