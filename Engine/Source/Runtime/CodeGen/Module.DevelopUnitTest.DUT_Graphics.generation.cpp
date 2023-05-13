#include "PCH/PCH.h"
#include "Core.h"
#include "Editor/DevelopUnitTest/DUT_Graphics/DUT_Graphics.h"


PSharedPtr<JGClass> Module_DevelopUnitTest_Code_Generation_Static_Create_Class_JGDUT_Graphics()
{
	HList<PSharedPtr<JGFunction>> FunctionMap;
	HList<PSharedPtr<JGProperty>> PropertyMap;
	return PObjectGlobalsPrivateUtils::MakeStaticClass(JGType::GenerateType<JGDUT_Graphics>(),{
			JGType::GenerateType<JGDevelopUnit>(), 
		},
		PropertyMap, FunctionMap,nullptr);
}

PSharedPtr<JGClass> Module_DevelopUnitTest_Code_Generation_Create_Class_JGDUT_Graphics(const JGObject* fromThis)
{
	JGDUT_Graphics* noneConstThisPtr = const_cast<JGDUT_Graphics*>(static_cast<const JGDUT_Graphics*>(fromThis));
	PSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(noneConstThisPtr, GObjectGlobalSystem::GetInstance().GetStaticClass(JGType::GenerateType<JGDUT_Graphics>()));
	return Class;

}

PSharedPtr<JGObject> Module_DevelopUnitTest_Code_Generation_Create_Object_JGDUT_Graphics()
{
	return Allocate<JGDUT_Graphics>();
}

