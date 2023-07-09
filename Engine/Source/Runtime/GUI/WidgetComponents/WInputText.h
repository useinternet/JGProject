#pragma once
#include "WidgetComponents/WidgetComponent.h"

class WInputText : public WWidgetComponent
{
public:
	struct HArguments
	{

	};

private:
	PString _inputText;
	int32   _maxBufferSize = 256;

public:
	WInputText() : WWidgetComponent() {}
	virtual ~WInputText() = default;

	void Construct(const HArguments& inArgs);

public:
	const PString& GetInputText() const;

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
};