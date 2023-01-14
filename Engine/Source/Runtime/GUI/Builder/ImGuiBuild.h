#pragma once
#include "GUIBuilder.h"


class PImGuiBuild final : public IMemoryObject, public IGUIBuild
{
private:
	virtual void OnBuild(const HGUIBuilder::HCommandData& inCommandData) override;

private:
	void OnBuildText(HGUIBuilder::PTextCommandValue* inCV);
};