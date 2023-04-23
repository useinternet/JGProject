#pragma once
#include "MenuCommand.h"

class HGenericMenuCommands : public HMenuCommands<HGenericMenuCommands>
{
public:
	HMenuCommand Copy;
	HMenuCommand Paste;
	HMenuCommand Cut;

public:
	void Register() {}
};