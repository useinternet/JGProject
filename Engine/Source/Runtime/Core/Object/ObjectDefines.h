#pragma once

#include "CoreDefines.h"


#define JGCLASS(...)
#define JGINTERFACE(...)
#define JGSTRUCT(...)
#define JGPROPERTY(...)
#define JGFUNCTION(...)
#define JGENUM(...)
#define JGENUMMETA(...)


#define JG_GENERATED_SIMPLE_BODY \
public:  \
	virtual JGType GetType() const override { return JGType::GenerateType(this); } \
\
private: \
\

#define JG_GENERATED_CLASS_BODY(...) \
	JG_GENERATED_SIMPLE_BODY \
	friend class PObjectGlobalsPrivateUtils; \