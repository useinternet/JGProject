#pragma once
#include "WidgetComponents/WidgetComponent.h"


class WProperty : public WWidgetComponent
{
public:
	struct HArguments
	{
		JGType PropertyType;

	};
private:
	JGType _propertyType;

public:
	virtual ~WProperty() = default;

	void Construct(const HArguments& inArgs);
protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
	virtual void OnTitle(HGUIBuilder& inBuilder);
	virtual void OnContent(HGUIBuilder& inBuilder) {}
};