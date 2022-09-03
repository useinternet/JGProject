#pragma once


#include "CoreDefines.h"


#define JGCLASS(...)
#define JGINTERFACE(...)
#define JGSTRUCT(...) 


#define JG_GENERATED_SIMPLE_BODY \
public:  \
	virtual JGType GetType() const override { return JGType::GenerateType(this); } \
private: \

#define JG_GENERATED_JGCLASS_BODY(...) \
	friend class PObjectGlobalsPrivateUtils; \
	JG_GENERATED_SIMPLE_BODY \




#define JGPROPERTY(...)
#define JGFUNCTION(...)
#define JGMETA(...)