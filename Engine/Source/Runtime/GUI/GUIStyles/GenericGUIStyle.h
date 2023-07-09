#pragma once
#include "GUIStyle.h"
#include "Runtime/CodeGen/Module.GUI.GenericGUIStyle.generation.h"

JGCLASS()
class JGGenericGUIStyle : public JGGUIStyle
{
	JG_GENERATED_CLASS_BODY

public:
	enum EStyleColor
	{
		Button,
		ButtonHovered,
		ButtonActive,
		Border,
		BorderHovered,
		BorderActive,
		BorderDisable,
	};



public:
	virtual ~JGGenericGUIStyle() = default;

private:
	virtual void Construct() override;

public:
	static JGGenericGUIStyle& Get();
};