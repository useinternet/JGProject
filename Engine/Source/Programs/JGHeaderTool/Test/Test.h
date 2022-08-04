#pragma once


#include "Core.h"




////// struct, class, interface, enum, enumflags
////
//////		+ Field  : 변수, 함수 정보, 사이즈
////
//
//
//
//
//// JGSTRUCT
//
//
//// -- GENERATED_JGSTRUCT_BODY
////#define GENERATED_JGSTRUCT_BODY \
////static void Generated_Function_Create_TestStruct() \
////{ \
////	AUTO_GENERATED_MACRO_JGTESTSTRUCT_GENERATE_TYPE\
////	AUTO_GENERATED_MACRO_JGTESTSTRUCT_GENERATE_FIELD\
////};\
//
//
//
//
//// -- Gen 파일에 생성되야할 것들
//// -- TestStruct.Gen.ixx
//
//#define AUTO_GENERATED_MACRO_JGTESTSTRUCT_CLASS_NAME "JGTestStruct";
//
////
////#define AUTO_GENERATED_MACRO_JGTESTSTRUCT_GENERATE_TYPE \
////PType type; \
////type.Name = AUTO_GENERATED_MACRO_JGTESTSTRUCT_CLASS_NAME;\
////type.ID = type.Name.GetStringTableID();
////
////
////
////
////#define AUTO_GENERATED_MACRO_JGTESTSTRUCT_GENERATE_FIELD \
////PField field; \
////\
////PProperty auto_generated_Test1;\
////auto_generated_Test1.Name = "Test1";\
////auto_generated_Test1.PropertyType = EPropertyType::Float;\
////\
////PProperty auto_generated_Test2;\
////auto_generated_Test1.Name = "Test2";\
////auto_generated_Test1.PropertyType = EPropertyType::Int32;\
////	\
////field.Properties.push_back(auto_generated_Test1);\
////field.Properties.push_back(auto_generated_Test2);\
//
////JGSTRUCT()
//struct JGTestStruct
//{
//	// --GENERATED_JGSTRUCT_BODY
//
//	static void Generated_Function_Create_TestStruct()
//	{
//		PType type;
//		type.Name = AUTO_GENERATED_MACRO_JGTESTSTRUCT_CLASS_NAME;
//		type.ID = type.Name.GetStringTableID();
//
//		PField field;
//
//		PProperty auto_generated_Test1;
//		auto_generated_Test1.Name = "Test1";
//		auto_generated_Test1.PropertyType = EPropertyType::Float;
//
//		PProperty auto_generated_Test2;
//		auto_generated_Test1.Name = "Test2";
//		auto_generated_Test1.PropertyType = EPropertyType::Int32;
//
//		field.Properties.push_back(auto_generated_Test1); 
//		field.Properties.push_back(auto_generated_Test2);
//
//		PStruct _struct;
//		_struct.Field = field;
//		_struct.Type  = type;
//		_struct.StructPtr = nullptr;
//
//		// "Reflection"
//		// ->SetMeta(var Name, data) 로 실시간 변환 가능
//		// ->CallFunction("Name") 으로 함수 호출 가능
//
//		// "enum의 경우"
//		// ToString, ToInt 자유롭게 가능
//	};
//	// 
//	//PStruct* GetStaticStruct()
//	//{
//
//	//}
//
//
//
//	// -- PROPERTY
//	JGPROPERTY();
//	float Test1;
//
//
//	JGPROPERTY();
//	int32 Test2;
//
//
//};
