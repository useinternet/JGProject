#include "CoreDefines.h"
#if JGCLASS
#undef JGCLASS(...)
#endif

#if JG_GENERATED_CLASS_BODY
#undef JG_GENERATED_CLASS_BODY()
#endif

#define JGCLASS(...)  \
class JGTestClassA;
extern PSharedPtr<JGClass> Module_Core_Code_Generation_Static_Create_Class_JGTestClassA()\
extern PSharedPtr<JGClass> Module_Core_Code_Generation_Create_Class_JGTestClassA(const JGTestClassA* fromThis)\

#define JG_GENERATED_CLASS_BODY() \
public: \
PSharedPtr<JGClass> GetClass() const\
{ \
	return PSharedPtr<JGClass> Module_Core_Code_Generation_Create_Class_JGTestClassA(this)\
} \
static PSharedPtr<JGStruct> GetStaticClass()\
{ \
	return GObjectGlobalSystem::GetInstance().GetStaticClass<JGTestClassA>();\
} \
