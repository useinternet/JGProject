#pragma once
#include "Core.h"
#include "JGEditorDefine.h"
#include "Runtime/CodeGen/Module.JGEditor.DevelopUnitData.generation.h"
// DevelopUnit

JGCLASS()
class JGDevelopUnitData : public JGObject
{
	JG_GENERATED_CLASS_BODY
public:
	JGPROPERTY()
	HGuid UnitGuid;







public:
	virtual ~JGDevelopUnitData() = default;
};