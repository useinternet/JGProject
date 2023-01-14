#pragma once
#include "Core.h"
#include "GUIDefine.h"

#define DEFINE_GUIBUILD_COMMAND_ONEVALUE(ClassName, type1, name1)\
class ClassName : public PCommandValue \
{ \
public: \
	type1 name1; \
	ClassName(const type1 & in##name1)\
	{\
		name1 = in##name1;\
	}\
	virtual ~##ClassName() = default;\
};\

#define DEFINE_GUIBUILD_COMMAND_TWOVALUE(ClassName, type1, name1, type2, name2)\
class ClassName : public PCommandValue \
{ \
public: \
	type1 name1; \
	type2 name2; \
	ClassName(const type1 & in##name1, const type2 & in##name2,)\
{\
name1 = in##name1; \
name2 = in##name2; \
}\
virtual ~##ClassName() = default; \
}; \

#define DEFINE_GUIBUILD_COMMAND_THREEVALUE(ClassName, type1, name1, type2, name2, type3, name3)\
class ClassName : public PCommandValue \
{ \
public: \
	type1 name1; \
	type2 name2; \
	type3 name3; \
	ClassName(const type1 & in##name1, const type2 & in##name2, const type3 & in##name3)\
{\
name1 = in##name1; \
name2 = in##name2; \
name3 = in##name3; \
}\
virtual ~##ClassName() = default; \
}; \

#define DEFINE_GUIBUILD_COMMAND_FOURVALUE(ClassName,type1, name1, type2, name2, type3, name3, type4, name4) \
class ClassName : public PCommandValue \
{ \
public: \
	type1 name1; \
	type2 name2; \
	type3 name3; \
	type4 name4; \
	ClassName(const type1 & in##name1, const type2 & in##name2, const type3 & in##name3, const type4 & in##name4)\
{\
name1 = in##name1; \
name2 = in##name2; \
name3 = in##name3; \
name4 = in##name4; \
}\
virtual ~##ClassName() = default; \
}; \

#define DEFINE_GUIBUILD_COMMAND_FIVEVALUE(ClassName,type1, name1, type2, name2, type3, name3, type4, name4, type5, name5) \
class ClassName : public PCommandValue \
{ \
public: \
	type1 name1; \
	type2 name2; \
	type3 name3; \
	type4 name4; \
	type5 name5; \
	ClassName(const type1 & in##name1, const type2 & in##name2, const type3 & in##name3, const type4 & in##name4, const type5 & in##name5)\
{\
name1 = in##name1; \
name2 = in##name2; \
name3 = in##name3; \
name4 = in##name4; \
name5 = in##name5; \
}\
virtual ~##ClassName() = default; \
}; \

class WWidget;
class IGUIBuild;
class HGUIBuilder
{
public:
	enum class ECommand
	{
		None,
		BeginHorizontal,
		EndHorizontal,
		BeginVertical,
		EndVertical,
		PushWidget,
		Text,
		Button,
	};

	class PCommandValue : public IMemoryObject
	{
	public:
		virtual ~PCommandValue() = default;
	};

	struct HCommandData
	{
		ECommand Command;
		PSharedPtr<PCommandValue> CommandValue;
	};

	DEFINE_GUIBUILD_COMMAND_ONEVALUE(PTextCommandValue, PString, Text)
	DEFINE_GUIBUILD_COMMAND_ONEVALUE(PWidgetCommandValue, PSharedPtr<WWidget>, Widget)
public:
	HGUIBuilder(PSharedPtr<IGUIBuild> inGuiBuildIF) : _guiBuild(inGuiBuildIF) {}

	void BeginHorizontal(int32 fixedWidth = INDEX_NONE);
	void EndHorizontal();

	void BeginVertical(int32 fixedHeight = INDEX_NONE);
	void EndVertical();

	void PushWidget(PSharedPtr<WWidget> inWidget);
	void Text(const PString& inText);

private:
	void Build();

private:
	PSharedPtr<IGUIBuild> _guiBuild;
	HQueue<HCommandData> _commandQueue;
};

class IGUIBuild
{
public:
	virtual void OnBuild(const HGUIBuilder::HCommandData& inCommandData) = 0;
};
