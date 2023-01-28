#pragma once
#include "WidgetComponents/WidgetComponent.h"

class WInputText : public WWidgetComponent
{
private:
	PString _inputText;

public:
	int32 MaxBufferSize = 256;

public:
	WInputText() : WWidgetComponent() {}
	virtual ~WInputText() = default;

public:
	const PString& GetInputText() const;


protected:
	virtual void GenerateImGuiWidgetComponent(const HWidgetContext& inWidgetContext) override;
};