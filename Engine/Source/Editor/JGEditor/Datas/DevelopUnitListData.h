#pragma once
#include "Core.h"
#include "JGEditorDefine.h"
#include "DevelopUnitData.h"
#include "Runtime/CodeGen/Module.JGEditor.DevelopUnitListData.generation.h"
// DevelopUnit

class JGDevelopUnit;

JGCLASS()
class JGDevelopUnitListData : public JGObject
{
	JG_GENERATED_CLASS_BODY
public:
	JGPROPERTY()
	HList<PSharedPtr<JGDevelopUnitData>> DevelopUnitDatas;

public:
	virtual ~JGDevelopUnitListData() = default;

	void AddUnitData(JGDevelopUnit* inDevelopUnit);
	void RemoveUnitData(JGDevelopUnit* inDevelopUnit);
};