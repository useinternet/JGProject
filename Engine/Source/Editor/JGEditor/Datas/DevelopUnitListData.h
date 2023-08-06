#pragma once
#include "Core.h"
#include "JGEditorDefine.h"
#include "DevelopUnitData.h"
#include "Runtime/CodeGen/Module.JGEditor.DevelopUnitListData.generation.h"
// DevelopUnit

JGCLASS()
class JGDevelopUnitListData : public JGObject
{
	JG_GENERATED_CLASS_BODY
public:
	JGPROPERTY()
	HList<JGDevelopUnitData> DevelopUnitDatas;

public:
	virtual ~JGDevelopUnitListData() = default;
};