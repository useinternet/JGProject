#ifdef JG_GENERATED_CLASS_BODY
#undef JG_GENERATED_CLASS_BODY
#endif

#define JG_GENERATED_CLASS_BODY \
friend class PObjectGlobalsPrivateUtils; \
JG_GENERATED_SIMPLE_BODY \
friend PSharedPtr<JGClass> Module_GUI_Code_Generation_Create_Class_WWidget(const JGObject* fromThis);\
friend PSharedPtr<JGClass> Module_GUI_Code_Generation_Static_Create_Class_WWidget();\
protected: \
 \
	virtual void WriteJson(PJsonData& json) const override \
	{ \
		__super::WriteJson(json); \
 \
		PJsonData dataJson = json.CreateJsonData(); \
\
    dataJson.AddMember("_widgetGuid", _widgetGuid); \
    dataJson.AddMember("_bIsOpen", _bIsOpen); \
\
\
		json.AddMember(JGType::GenerateType<WWidget>().GetName().ToString(), dataJson);\
	}\
	virtual void ReadJson(const PJsonData& json) override\
	{\
		__super::ReadJson(json);\
\
		PJsonData dataJson;\
		if (json.FindMember(JGType::GenerateType<WWidget>().GetName().ToString(), &dataJson) == false)\
		{\
			return;\
		}\
\
		if (dataJson.GetData("_widgetGuid", &_widgetGuid) == false)\
		{\
			\
		}\
\
\
		if (dataJson.GetData("_bIsOpen", &_bIsOpen) == false)\
		{\
			\
		}\
\
\
\
	}\
private:\
\
