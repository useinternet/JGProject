#include "pch.h"
#include "GameBridge.h"


void Start_Plugin(JG::SharedPtr<JG::GamePluginLinker> linker)
{
	if (linker == nullptr)
	{
		return;
	}
	linker->Apply();
}
