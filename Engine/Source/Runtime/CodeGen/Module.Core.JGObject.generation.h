#ifdef JG_GENERATED_CLASS_BODY
#undef JG_GENERATED_CLASS_BODY
#endif

#define JG_GENERATED_CLASS_BODY \
friend class PObjectGlobalsPrivateUtils; \
JG_GENERATED_SIMPLE_BODY \
friend PSharedPtr<JGClass> Module_Core_Code_Generation_Create_Class_JGObject(const JGObject* fromThis);\
friend PSharedPtr<JGClass> Module_Core_Code_Generation_Static_Create_Class_JGObject();\
protected: \
 \
	virtual void WriteJson(PJsonData& json) const override \
	{ \
		__super::WriteJson(json); \
 \
		PJsonData dataJson = json.CreateJsonData(); \
\
\
\
		json.AddMember(JGType::GenerateType<JGObject>().GetName().ToString(), dataJson);\
	}\
	virtual void ReadJson(const PJsonData& json) override\
	{\
		__super::ReadJson(json);\
\
		PJsonData dataJson;\
		if (json.FindMember(JGType::GenerateType<JGObject>().GetName().ToString(), &dataJson) == false)\
		{\
			return;\
		}\
\
\
	}\
private:\
\
