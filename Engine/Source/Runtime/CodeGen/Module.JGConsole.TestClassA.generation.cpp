#include "PCH/PCH.h"
#include "Core.h"
#include "Programs/JGConsole/TestClass/TestClassA.h"


PSharedPtr<JGClass> Module_JGConsole_Code_Generation_Static_Create_Class_JGTestClassA()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGType::GenerateType<int32>(),"TestValue1",PObjectGlobalsPrivateUtils::MakeStaticMeta(
{
		HPair<PName, HHashSet<PName>>("Category", {PName("Setting"),		}),
		HPair<PName, HHashSet<PName>>("Meta", {PName("Test1"),PName("Test2"),PName("Test3"),		}),
	})
));
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGType::GenerateType<float32>(),"TestValue2",PObjectGlobalsPrivateUtils::MakeStaticMeta(
{
		HPair<PName, HHashSet<PName>>("Category", {PName("Setting"),		}),
		HPair<PName, HHashSet<PName>>("Meta", {PName("Test4"),PName("Test5"),PName("Test6"),		}),
	})
));
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGType::GenerateType<PString>(),"TestValue3",PObjectGlobalsPrivateUtils::MakeStaticMeta(
{
		HPair<PName, HHashSet<PName>>("Category", {PName("Setting"),PName("Hidden"),		}),
		HPair<PName, HHashSet<PName>>("Hidden", {PName("Hidden"),		}),
	})
));
	FunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction("GetTestFunc",
		JGType::GenerateType<float>(), {}, 
		PObjectGlobalsPrivateUtils::MakeStaticMeta(
		{
			HPair<PName, HHashSet<PName>>("Category", {PName("Setting"), 			}), 
		})));
	FunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction("TestFuncFunc",
		JGType::GenerateType<void>(), {JGType::GenerateType<float>(), JGType::GenerateType<float>(), JGType::GenerateType<int32>(), }, 
		PObjectGlobalsPrivateUtils::MakeStaticMeta(
		{
			HPair<PName, HHashSet<PName>>("Hidden", {PName("Hidden"), 			}), 
			HPair<PName, HHashSet<PName>>("Category", {PName("Setting"), PName("Hidden"), 			}), 
		})));
	FunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction("ExeFunc",
		JGType::GenerateType<void>(), {}, 
		PObjectGlobalsPrivateUtils::MakeStaticMeta(
		{
			HPair<PName, HHashSet<PName>>("exec", {PName("exec"), 			}), 
			HPair<PName, HHashSet<PName>>("Category", {PName("Setting"), PName("Hidden"), 			}), 
		})));
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGType::GenerateType<JGTestClassA>(),{
			JGType::GenerateType<JGObject>(), 
		},
		PropertyMap, FunctionMap,PObjectGlobalsPrivateUtils::MakeStaticMeta({
			HPair<PName, HHashSet<PName>>("Category", {PName("TestClass"),}),
			HPair<PName, HHashSet<PName>>("Config", {PName("JGConfig"),}),
		}));

}

PSharedPtr<JGClass> Module_JGConsole_Code_Generation_Create_Class_JGTestClassA(const JGObject* fromThis)
{
	JGTestClassA* noneConstThisPtr = const_cast<JGTestClassA*>(static_cast<const JGTestClassA*>(fromThis));
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(noneConstThisPtr, GObjectGlobalSystem::GetInstance().GetStaticClass(JGType::GenerateType<JGTestClassA>()));

	if (Class->HasProperty(PName("TestValue1")) == true)
	{
		if (PObjectGlobalsPrivateUtils::BindProperty(noneConstThisPtr, Class->FindProperty(PName("TestValue1")), &(noneConstThisPtr->TestValue1)) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGTestClassA: Fail Bind Property : TestValue1");
		}
	}



	if (Class->HasProperty(PName("TestValue2")) == true)
	{
		if (PObjectGlobalsPrivateUtils::BindProperty(noneConstThisPtr, Class->FindProperty(PName("TestValue2")), &(noneConstThisPtr->TestValue2)) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGTestClassA: Fail Bind Property : TestValue2");
		}
	}



	if (Class->HasProperty(PName("TestValue3")) == true)
	{
		if (PObjectGlobalsPrivateUtils::BindProperty(noneConstThisPtr, Class->FindProperty(PName("TestValue3")), &(noneConstThisPtr->TestValue3)) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGTestClassA: Fail Bind Property : TestValue3");
		}
	}


	if(Class->HasFunction(PName("GetTestFunc")) == true)
	{
		HDelegate<float> funcDelegate; funcDelegate.BindRaw(noneConstThisPtr, &JGTestClassA::GetTestFunc);
		if (PObjectGlobalsPrivateUtils::BindFunction(noneConstThisPtr, Class->FindFunction(PName("GetTestFunc")), funcDelegate, {}) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGTestClassA: Fail Bind Function : GetTestFunc");
		}
	}
	if(Class->HasFunction(PName("TestFuncFunc")) == true)
	{
		HDelegate<void,float,float,int32> funcDelegate; funcDelegate.BindRaw(noneConstThisPtr, &JGTestClassA::TestFuncFunc);
		if (PObjectGlobalsPrivateUtils::BindFunction(noneConstThisPtr, Class->FindFunction(PName("TestFuncFunc")), funcDelegate, {JGType::GenerateType<float>(),JGType::GenerateType<float>(),JGType::GenerateType<int32>()}) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGTestClassA: Fail Bind Function : TestFuncFunc");
		}
	}
	if(Class->HasFunction(PName("ExeFunc")) == true)
	{
		HDelegate<void> funcDelegate; funcDelegate.BindRaw(noneConstThisPtr, &JGTestClassA::ExeFunc);
		if (PObjectGlobalsPrivateUtils::BindFunction(noneConstThisPtr, Class->FindFunction(PName("ExeFunc")), funcDelegate, {}) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGTestClassA: Fail Bind Function : ExeFunc");
		}
	}
	return Class;

}

PSharedPtr<JGObject> Module_JGConsole_Code_Generation_Create_Object_JGTestClassA()
{
	return Allocate<JGTestClassA>();
}

PSharedPtr<JGEnum> Module_JGConsole_Code_Generation_Static_Create_Enum_ETestEnumA()
{

	HList<PSharedPtr<JGMeta>> MetaList;
	HList<PName> EnumStringList;
	HHashMap<int32, int32> EnumRedirectMap;

	MetaList.resize(6);
	EnumStringList.resize(6);

    EnumStringList[0] = "AAA";
    EnumRedirectMap[0] = 0;


	MetaList[0] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Category", {PName("Hidden"),}),
            HPair<PName, HHashSet<PName>>("Hidden", {PName("Hidden"),}),

	});

    EnumStringList[1] = "BBB";
    EnumRedirectMap[10] = 1;


	MetaList[1] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Display", {PName("Display"),}),
            HPair<PName, HHashSet<PName>>("Category", {PName("Display"),}),

	});

    EnumStringList[2] = "CCC";
    EnumRedirectMap[11] = 2;


	MetaList[2] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[3] = "DDD";
    EnumRedirectMap[12] = 3;


	MetaList[3] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Hidden", {PName("Hidden"),}),
            HPair<PName, HHashSet<PName>>("Category", {PName("Hidden"),}),

	});

    EnumStringList[4] = "EEE";
    EnumRedirectMap[30] = 4;


	MetaList[4] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Display", {PName("Display"),}),
            HPair<PName, HHashSet<PName>>("Category", {PName("Display"),}),

	});

    EnumStringList[5] = "MMM";
    EnumRedirectMap[31] = 5;


	MetaList[5] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("EXEC", {PName("EXEC"),}),

	});

    return PObjectGlobalsPrivateUtils::MakeStaticEnum(JGType::GenerateType<ETestEnumA>(), "ETestEnumA", EnumRedirectMap, EnumStringList , MetaList);
}


