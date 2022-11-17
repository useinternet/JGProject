#include "Core.h"
#include "Programs/JGConsole/TestClass/TestClassA.h"


PSharedPtr<JGClass> Module_JGConsole_Code_Generation_Static_Create_Class_JGTestClassA()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(int32),"TestValue1",PObjectGlobalsPrivateUtils::MakeStaticMeta(
{
		HPair<PName, HHashSet<PName>>("Category", {PName("Setting"),		}),
		HPair<PName, HHashSet<PName>>("Meta", {PName("Test1"),PName("Test2"),PName("Test3"),		}),
	})
));
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(float32),"TestValue2",PObjectGlobalsPrivateUtils::MakeStaticMeta(
{
		HPair<PName, HHashSet<PName>>("Category", {PName("Setting"),		}),
		HPair<PName, HHashSet<PName>>("Meta", {PName("Test4"),PName("Test5"),PName("Test6"),		}),
	})
));
	PropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(PString),"TestValue3",PObjectGlobalsPrivateUtils::MakeStaticMeta(
{
		HPair<PName, HHashSet<PName>>("Category", {PName("Setting"),PName("Hidden"),		}),
		HPair<PName, HHashSet<PName>>("Hidden", {PName("Hidden"),		}),
	})
));
	FunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction("GetTestFunc",
		JGTYPE(float), {}, 
		PObjectGlobalsPrivateUtils::MakeStaticMeta(
		{
			HPair<PName, HHashSet<PName>>("Category", {PName("Setting"), 			}), 
		})));
	FunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction("TestFuncFunc",
		JGTYPE(void), {JGTYPE(float), JGTYPE(float), JGTYPE(int32), }, 
		PObjectGlobalsPrivateUtils::MakeStaticMeta(
		{
			HPair<PName, HHashSet<PName>>("Hidden", {PName("Hidden"), 			}), 
			HPair<PName, HHashSet<PName>>("Category", {PName("Setting"), PName("Hidden"), 			}), 
		})));
	FunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction("ExeFunc",
		JGTYPE(void), {}, 
		PObjectGlobalsPrivateUtils::MakeStaticMeta(
		{
			HPair<PName, HHashSet<PName>>("exec", {PName("exec"), 			}), 
			HPair<PName, HHashSet<PName>>("Category", {PName("Setting"), PName("Hidden"), 			}), 
		})));
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGTYPE(JGTestClassA),{
			JGTYPE(JGObject), 
		},
		PropertyMap, FunctionMap,PObjectGlobalsPrivateUtils::MakeStaticMeta({
			HPair<PName, HHashSet<PName>>("Category", {PName("TestClass"),}),
			HPair<PName, HHashSet<PName>>("Config", {PName("JGConfig"),}),
		}));

}

PSharedPtr<JGClass> Module_JGConsole_Code_Generation_Create_Class_JGTestClassA(const JGObject* fromThis)
{
	JGTestClassA* noneConstThisPtr = const_cast<JGTestClassA*>(static_cast<const JGTestClassA*>(fromThis));
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(noneConstThisPtr, GObjectGlobalSystem::GetInstance().GetStaticClass(JGTYPE(JGTestClassA)));

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
		std::function<float()> functionRef = std::bind(&JGTestClassA::GetTestFunc, noneConstThisPtr);
		if (PObjectGlobalsPrivateUtils::BindFunction(noneConstThisPtr, Class->FindFunction(PName("GetTestFunc")), functionRef, {}) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGTestClassA: Fail Bind Function : GetTestFunc");
		}
	}
	if(Class->HasFunction(PName("TestFuncFunc")) == true)
	{
		std::function<void(float,float,int32)> functionRef = std::bind(&JGTestClassA::TestFuncFunc, noneConstThisPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		if (PObjectGlobalsPrivateUtils::BindFunction(noneConstThisPtr, Class->FindFunction(PName("TestFuncFunc")), functionRef, {JGTYPE(float),JGTYPE(float),JGTYPE(int32)}) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGTestClassA: Fail Bind Function : TestFuncFunc");
		}
	}
	if(Class->HasFunction(PName("ExeFunc")) == true)
	{
		std::function<void()> functionRef = std::bind(&JGTestClassA::ExeFunc, noneConstThisPtr);
		if (PObjectGlobalsPrivateUtils::BindFunction(noneConstThisPtr, Class->FindFunction(PName("ExeFunc")), functionRef, {}) == false)
		{
			JG_LOG(CodeGen, ELogLevel::Error, "JGTestClassA: Fail Bind Function : ExeFunc");
		}
	}
	return Class;

}

PSharedPtr<JGEnum> Module_JGConsole_Code_Generation_Static_Create_Enum_ETestEnumA()
{

	HList<PSharedPtr<JGMeta>> MetaList;
	HList<PName> EnumStringList;


	MetaList.resize(6);
	EnumStringList.resize(6);

    EnumStringList[0] = "AAA";


	MetaList[0] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Category", {PName("Hidden"),}),
            HPair<PName, HHashSet<PName>>("Hidden", {PName("Hidden"),}),

	});

    EnumStringList[1] = "BBB";


	MetaList[1] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Display", {PName("Display"),}),
            HPair<PName, HHashSet<PName>>("Category", {PName("Display"),}),

	});

    EnumStringList[2] = "CCC";


	MetaList[2] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[3] = "DDD";


	MetaList[3] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Hidden", {PName("Hidden"),}),
            HPair<PName, HHashSet<PName>>("Category", {PName("Hidden"),}),

	});

    EnumStringList[4] = "EEE";


	MetaList[4] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Display", {PName("Display"),}),
            HPair<PName, HHashSet<PName>>("Category", {PName("Display"),}),

	});

    EnumStringList[5] = "MMM";


	MetaList[5] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("EXEC", {PName("EXEC"),}),

	});

    return PObjectGlobalsPrivateUtils::MakeStaticEnum(JGTYPE(ETestEnumA), "ETestEnumA", EnumStringList ,MetaList);
}


