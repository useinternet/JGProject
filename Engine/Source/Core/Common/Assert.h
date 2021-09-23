#pragma once


#include "Log.h"


#ifdef _DEBUG


#define JGASSERT_IF(check, msg) \
{ \
	if (!(check)) assert(msg); \
} \

#define JGASSERT(msg)	assert(msg);
#else
#define JGASSERT_IF
#define JGASSERT(msg)
#endif

