#include "Core.h"
#include "Test/TestObject.h"

PSharedPtr<JGClass> Module_Core_Code_Generation_Static_Create_Class_PTestObject()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int),"TestValue1",nullptr));
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int32),"TestValue3",nullptr));
	FunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction("TestFunc",
		PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(void), ""),
		{
			PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int), "TestValue1"),
			PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int32), "TestValue3"),
		},
		PObjectGlobalsPrivateUtils::MakeStaticMeta(
		{
		})));
	FunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction("TestFunc2",
		PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int32), ""),
		{
			PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int), "TestValue1"),
			PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int32), "TestValue3"),
		},
		PObjectGlobalsPrivateUtils::MakeStaticMeta(
		{
			HPair<PString, HHashSet<PString>>("TestMetaFunc", {"TestMetaFunc", 			}), 
		})));
	FunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction("TestFunc3",
		PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(float32), ""),
		{
			PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int), "TestValue1"),
			PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int32), "TestValue3"),
		},
		PObjectGlobalsPrivateUtils::MakeStaticMeta(
		{
		})));
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGTYPE(PTestObject),{
			JGTYPE(JGObject), 
		},
		PropertyMap, FunctionMap,PObjectGlobalsPrivateUtils::MakeStaticMeta({
			HPair<PString, HHashSet<PString>>("TEST", {"TEST",}),
			HPair<PString, HHashSet<PString>>("Category", {"asoijfd","aosdifjaosdf",}),
		}));

}

PSharedPtr<JGClass> Module_Core_Code_Generation_Create_Class_PTestObject(const PTestObject* fromThis)
{
	PTestObject* noneConstThisPtr = const_cast<PTestObject*>(fromThis);
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(fromThis, GObjectGlobalSystem::GetInstance().GetStaticClass(JGTYPE(PTestObject)));
	if(Class->HasFunction(PName("TestFunc")) == true)
	{
		std::function<void()> functionRef = std::bind(&PTestObject::TestFunc, noneConstThisPtr);
		if (PObjectGlobalsPrivateUtils::BindFunction(fromThis, Class->FindFunction(PName("TestFunc")), functionRef) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "PTestObject: Fail Bind Function : TestFunc");
		}
	}
	if(Class->HasFunction(PName("TestFunc2")) == true)
	{
		std::function<int32(float32,int32)> functionRef = std::bind(&PTestObject::TestFunc2, noneConstThisPtr, std::placeholders::_1, std::placeholders::_2);
		if (PObjectGlobalsPrivateUtils::BindFunction(fromThis, Class->FindFunction(PName("TestFunc2")), functionRef) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "PTestObject: Fail Bind Function : TestFunc2");
		}
	}
	if(Class->HasFunction(PName("TestFunc3")) == true)
	{
		std::function<float32(const PString&)> functionRef = std::bind(&PTestObject::TestFunc3, noneConstThisPtr, std::placeholders::_1);
		if (PObjectGlobalsPrivateUtils::BindFunction(fromThis, Class->FindFunction(PName("TestFunc3")), functionRef) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "PTestObject: Fail Bind Function : TestFunc3");
		}
	}
	return Class;

}

