#pragma once


#include "CoreDefines.h"




#define JGCLASS(...) 
#define JGSTRUCT(...) 


#define GENERATED_SIMPLE_BODY \
public:  \
	virtual JGType GetType() const override { return JGType::GenerateType(this); } \
private: \

#define GENERATED_JGCLASS_BODY(...) \
	friend class PObjectGlobalsPrivateUtils; \




#define JGPROPERTY(...)
#define JGFUNCTION(...)
#define JGMETA(...)