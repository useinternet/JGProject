#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"
#include "Misc/Module.h"


class PJGGraphicsAPI;
class GRAPHICS_API HJGGraphicsModule : public IModuleInterface
{

	PSharedPtr<PJGGraphicsAPI> _graphicsAPI;
protected:
	JGType GetModuleType() const override;

	void StartupModule() override;
	void ShutdownModule() override;

private:
	void BeginFrame(const PTaskArguments& args);
	void EndFrame(const PTaskArguments& args);
public:
	PSharedPtr<PJGGraphicsAPI> GetGraphicsAPI() const;
	// Module ø° RenderScene ø‰√ª

	

	// RenderScene
	//
};

