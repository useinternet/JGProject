#pragma once
#include "WProperty.h"



class WProperty_Bool : public WProperty
{
public:
	struct HArguments
	{
		PWeakPtr<JGProperty> Property;
	};

private:
	PWeakPtr<JGProperty> _property;

public:
	WProperty_Bool() = default;
	virtual ~WProperty_Bool() = default;

	void Construct(const HArguments& inArgs);
};