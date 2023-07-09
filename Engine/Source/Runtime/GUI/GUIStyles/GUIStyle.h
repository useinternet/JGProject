#pragma once
#include "Core.h"
#include "Runtime/CodeGen/Module.GUI.GUIStyle.generation.h"

JGCLASS()
class JGGUIStyle : public JGObject
{
	JG_GENERATED_CLASS_BODY

private:
	HMap<uint32, HLinearColor> _styleColorMap;

public:
	virtual ~JGGUIStyle() = default;

protected:
	void SetStyleColor(uint32 Index, const HLinearColor& Color)
	{
		_styleColorMap[Index] = Color;
	}

public:
	virtual const HLinearColor& GetStyleColor(uint32 Index) const
	{
		return _styleColorMap.at(Index);
	}
};