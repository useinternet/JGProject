#pragma once
#include "Widget.h"
#include "JGEditorDefine.h"



class WDetailsView : public WWidget
{
public:
	struct HArguments
	{
		HList<PWeakPtr<JGObject>> DetailsObjects;
	};

private:
	HList<PWeakPtr<JGObject>> _detailsObjects;

public:
	WDetailsView() = default;
	virtual ~WDetailsView() = default;

	void Construct(const HArguments& inArgs);

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
};