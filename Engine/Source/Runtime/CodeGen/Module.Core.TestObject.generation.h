#ifdef JG_GENERATED_CLASS_BODY
#undef JG_GENERATED_CLASS_BODY
#endif

#define JG_GENERATED_CLASS_BODY \
friend class PObjectGlobalsPrivateUtils; \
JG_GENERATED_SIMPLE_BODY \
friend PSharedPtr<JGClass> Module_Core_Code_Generation_Create_Class_JGTestObject(const JGObject* fromThis);\
friend PSharedPtr<JGClass> Module_Core_Code_Generation_Static_Create_Class_JGTestObject();\
protected: \
 \
	virtual void WriteJson(PJsonData& json) const override \
	{ \
		__super::WriteJson(json); \
 \
		PJsonData dataJson = json.CreateJsonData(); \
\
    dataJson.AddMember("TestValue1", TestValue1); \
    dataJson.AddMember("TestValue3", TestValue3); \
\
\
		json.AddMember(JGTYPE(JGTestObject).GetName().ToString(), dataJson);\
	}\
	virtual void ReadJson(const PJsonData& json) override\
	{\
		__super::ReadJson(json);\
\
		PJsonData dataJson;\
		if (json.FindMember(JGTYPE(JGTestObject).GetName().ToString(), &dataJson) == false)\
		{\
			return;\
		}\
\
		if (dataJson.GetData("TestValue1", &TestValue1) == false)\
		{\
			\
		}\
\
\
		if (dataJson.GetData("TestValue3", &TestValue3) == false)\
		{\
			\
		}\
\
\
\
	}\
private:\
\
