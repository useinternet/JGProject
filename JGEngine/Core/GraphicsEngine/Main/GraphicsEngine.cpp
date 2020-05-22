#include "pch.h"
#include "GraphicsEngine.h"
#include "Object/GraphicsDevice.h"
REGISTER_GRAPHICSENGINE(GE::GraphicsEngine);



GE::GraphicsEngine* g_GE = nullptr;
Plugin plugin;
using TestFunc = void* (*)(HWND,int, int);

extern "C" __declspec(dllexport)
void BindUnitTest(HWND hwnd, int width, int height)
{
	//plugin.Load("DirectX12.dll");
	//((TestFunc)plugin.GetProcAddress("TestSendHWND"))(hwnd, width, height);
}

GE::Scene* GE::GraphicsEngine::RequestCreateScene()
{
	return nullptr;
}

void GE::GraphicsEngine::DestroyCreateScene(Scene* scene)
{
}

GE::Screen* GE::GraphicsEngine::RequestCreateScreen()
{
	return nullptr;
}

void GE::GraphicsEngine::RequestDestroyScreen(Screen* screen)
{
}

bool GE::GraphicsEngine::Engine_Create()
{
	switch (g_GE->m_Desc.api)
	{
	case GraphicsAPI::DirectX11:
		assert("not imp DirectX11");
		break;
	case GraphicsAPI::DirectX12:
		g_GE->m_APIPlugin.Load(g_GE->m_Desc.dx12DllPath);
		break;
	case GraphicsAPI::OpenGL:
		assert("not imp OpenGL");
		break;
	}


	
	((RegisterLogFunc)g_GE->m_APIPlugin.GetProcAddress("GE_Register_GlobalData"))(GlobalLinkData::GetStream());
	g_GE->m_GDevice = ((CreateGraphicsDeviceFunc)g_GE->m_APIPlugin.GetProcAddress("GE_Create_DeviceClass"))();
	g_GE->m_GDevice->Init();

	return false;
}

bool GE::GraphicsEngine::Engine_Load()
{
	if (!g_GE->m_GDevice->Load()) return false;




	return true;
}
void GE::GraphicsEngine::Engine_Update()
{
	g_GE->m_GDevice->Update();
}
void GE::GraphicsEngine::Engine_Destroy()
{
	if (g_GE->m_GDevice)
	{
		delete g_GE->m_GDevice;
		g_GE->m_GDevice = nullptr;

	}

}
void GE::GraphicsEngine::DestroyRObject(RObject* object)
{


}
GE::GraphicsEngine::GraphicsEngine(const GlobalLinkStream& stream) :   EngineCore(stream)
{
	if (g_GE == nullptr) g_GE = this;
	else assert("GraphicsEngine must have only one class");

}

void GE::GraphicsEngine::Init()
{
	Engine_Create();


}

void GE::GraphicsEngine::Load()
{
	Engine_Load();

}

void GE::GraphicsEngine::Update()
{
	Engine_Update();
}

void GE::GraphicsEngine::Destroy()
{
	Engine_Destroy();
}
