#pragma once
#include "Core.h"
#include "Misc/Module.h"
#include "JGEditorDefine.h"

class PJWindow;
class JGEDITOR_API HJGEditorModule : public IModuleInterface
{
	PSharedPtr<PJWindow> _window;
public:
	virtual ~HJGEditorModule();

protected:
	virtual JGType GetModuleType() const override;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

