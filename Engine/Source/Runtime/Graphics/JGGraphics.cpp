#include "PCH/PCH.h"
#include "JGGraphics.h"
#include "JGGraphicsAPI.h"

#ifdef _DIRECTX12
#include "DirectX12/DirectX12API.h"

#endif

JG_MODULE_IMPL(HJGGraphicsModule, GRAPHICS_C_API)

JGType HJGGraphicsModule::GetModuleType() const
{
	return JGTYPE(HJGGraphicsModule);
}

void HJGGraphicsModule::StartupModule()
{
	// 나중에 Engine Setting 값으로 뺄 예정
	// 일단 DirectX12 API 원픽
#ifdef _DIRECTX12
	_graphicsAPI = Allocate<PDirectX12API>();

	//_graphicsAPI
#else
	JG_ASSERT("not supported graphics api");
#endif
	
	HJGGraphicsArguments arguments;
	arguments.Handle = (uint64)GCoreSystem::GetGlobalValues().WindowHandle;
	arguments.Width = 1920;
	arguments.Height = 1080;
	arguments.ClearColor = HLinearColor(0, 0, 0, 0);
	arguments.BufferCount = 3;

	_graphicsAPI->Initialize(arguments);

	GScheduleGlobalSystem::GetInstance().ScheduleByFrame(_graphicsAPI, EMainThreadExecutionOrder::GraphicsBegin, SCHEDULE_FN_BIND(HJGGraphicsModule::BeginFrame));
	GScheduleGlobalSystem::GetInstance().ScheduleByFrame(_graphicsAPI, EMainThreadExecutionOrder::GraphicsEnd, SCHEDULE_FN_BIND(HJGGraphicsModule::EndFrame));

	JG_LOG(Graphics, ELogLevel::Trace, "Startup Graphics Module...");
}

void HJGGraphicsModule::ShutdownModule()
{
	_graphicsAPI = nullptr;

	JG_LOG(Graphics, ELogLevel::Trace, "Shutdown Graphics Module...");
}

void HJGGraphicsModule::BeginFrame(const PTaskArguments& args)
{
	if (_graphicsAPI == nullptr)
	{
		return;
	}

	_graphicsAPI->BeginFrame();
}

void HJGGraphicsModule::EndFrame(const PTaskArguments& args)
{
	if (_graphicsAPI == nullptr)
	{
		return;
	}

	_graphicsAPI->EndFrame();
}

PSharedPtr<PJGGraphicsAPI> HJGGraphicsModule::GetGraphicsAPI() const
{
	return _graphicsAPI;
}
