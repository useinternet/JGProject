#pragma once

#include "CommonCore.h"

#include "GUIIF.h"
#include "Components/Component.h"
#include "Elements/Element.h"
#include "Windows/Form.h"

#ifdef JGEDITOR_EXPORT

#define JGEDITOR_API __declspec(dllexport)

#else

#define JGEDITOR_API __declspec(dllimport)

#endif