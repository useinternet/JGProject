



#include <iostream>
#include "Core.h"
#include "TestClass/TestClassA.h"
// Test 내용

// Class, Interface 작동 여부
// Cast 작동 여부
// 메모리 누수 여부
// Enum , String/Meta Data 확인 여부

using namespace std;

int main()
{
	GCoreSystem::Create();
	
	PSharedPtr<JGObject> Object = Allocate<JGObject>();
	int32 testEnum = 0;



	GScheduleGlobalSystem::GetInstance().ScheduleOnce(Object, EMainThreadExecutionOrder::Begin, 
		[&](const PTaskArguments& Args)
	{
		JG_LOG(TEST, ELogLevel::Info, "Test Log");

		PSharedPtr<JGClass> Class = StaticClass<JGTestClassA>();

		PName TypeName = Class->GetClassType()->GetName();

		for (const PSharedPtr<JGProperty>& Property : Class->GetPropertyList())
		{
			PName PropertyTypeName = Property->GetPropertyType().GetName();

			PSharedPtr<JGMeta> Meta = Property->GetMeta();

			//	JGPROPERTY(Category = Setting, Meta = {Test1, Test2, Test3})
			bool testBool = Meta->HasMeta(PName("Hidden"));
			bool testBool2 = Meta->HasMeta(PName("Category"), PName("Setting"));

			int n = 0;
		}

		PSharedPtr<JGTestClassA> TestClass = Allocate< JGTestClassA>();

		PSharedPtr<JGClass> BindedClass = ::BindedClass<JGTestClassA>(TestClass.GetRawPointer());

		if (BindedClass != nullptr)
		{
			int32 Value = 0;
			BindedClass->GetPropertyValue(PName("TestValue1"), Value);

			float Value2 = 0.0f;
			BindedClass->GetPropertyValue(PName("TestValue2"), Value2);
			PString Value3 = "";
			BindedClass->GetPropertyValue(PName("TestValue3"), Value3);

			PSharedPtr<JGFunction> Func = BindedClass->FindFunction(PName("TestFuncFunc"));
			Func->Invoke<void>(0.0f, 1.0f, 5);
		}

	});



	while (true)
	{
		GCoreSystem::Update();
	}

	GCoreSystem::Destroy();
	return 0;
}


