#pragma once
#include "Core.h"
#include "Misc/Module.h"
#include "JGEditorDefine.h"


class JGEDITOR_API HJGEditorModule : public IModuleInterface
{

protected:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

