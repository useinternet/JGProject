#pragma once

#include "CoreDefines.h"
#include "String/String.h"



class PFileHelper
{

public:
	static bool WriteAllText(const PString& path, const PString& str);
	static bool ReadAllText(const PString& path, PString* out_str);
};