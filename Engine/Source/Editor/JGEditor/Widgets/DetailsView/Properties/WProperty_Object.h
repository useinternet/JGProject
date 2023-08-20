#pragma once
#include "WProperty.h"


class WProperty_Object : public WProperty
{
	struct HArguments : public WProperty::HArguments
	{

	};

public:
	virtual ~WProperty_Object() = default;

	void Construct(const HArguments& inArgs);

public:

	virtual void OnContent(HGUIBuilder& inBuilder) override;
};