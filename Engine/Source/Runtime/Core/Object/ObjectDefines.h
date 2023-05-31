#pragma once

#include "CoreDefines.h"


#define JGCLASS(...)
#define JGINTERFACE(...)
#define JGSTRUCT(...)
#define JGPROPERTY(...)
#define JGFUNCTION(...)
#define JGENUM(...)
#define JGENUMMETA(...)

#define TYPE_NULL_ID -1

#define JGTYPE(x)    JGType::GenerateType<##x>()
#define JGTYPEID(x)  JGType::GenerateTypeID<##x>()

#define JG_GENERATED_SIMPLE_BODY \
public:  \
	virtual JGType GetType() const override { return JGType::GenerateType(this); } \
\
private: \
\

#ifdef JG_GENERATED_CLASS_BODY
#undef JG_GENERATED_CLASS_BODY
#endif

#define JG_GENERATED_CLASS_BODY \
	friend class PObjectGlobalsPrivateUtils; \
	JG_GENERATED_SIMPLE_BODY \

	