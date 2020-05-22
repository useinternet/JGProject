#pragma once

#include "Core.h"

namespace GE
{
	class Scene; // 
	class Screen;
	class GraphicsDevice;
	class RObject;
	class Shader;
	
	enum class GraphicsAPI
	{
		DirectX12,
		DirectX11,
		OpenGL
	};
	class GraphicsEngineDesc
	{
	public:
		GraphicsAPI api = GraphicsAPI::DirectX12;
		std::string dx12DllPath   = "DirectX12.dll";
		std::string dx11DllPath   = "DirectX11.dll";
		std::string openglDllPath = "OpenGL.dll";
	};
	class GRAPHICSENGINE_API GraphicsEngine : public EngineCore
	{
	public:
		// Scene 관련
		static Scene* RequestCreateScene();
		static void   DestroyCreateScene(Scene* scene);


		// Screen 관련
		static Screen* RequestCreateScreen();
		static void    RequestDestroyScreen(Screen* screen);

		// Compile // hlsl, glsl 셰이더
		//static Shader* CompileShader();
		


		// Engine 관련
		static bool Engine_Create();
		static bool Engine_Load();
		static void Engine_Update();
		static void Engine_Destroy();



		//

		template<typename RObjectType>
		static RObject* CreateRObject();
		static void     DestroyRObject(RObject* object);
	public:
		GraphicsEngine(const GlobalLinkStream& stream);


	public:
		virtual void Init() override;
		virtual void Load() override;
		virtual void Update() override;
		virtual void Destroy() override;
	private:
		Plugin             m_APIPlugin;
		GraphicsEngineDesc m_Desc;
		GraphicsDevice*    m_GDevice = nullptr;


	private:
		using RegisterLogFunc = void* (*)(const GlobalLinkStream&);
		using CreateGraphicsDeviceFunc = GraphicsDevice* (*)();
	
	};

	template<typename RObjectType>
	inline RObject* GraphicsEngine::CreateRObject()
	{
		return (RObject*)(new RObjectType);
	}
}

#define GE_REGISTER_GLOBALDATA \
extern "C" __declspec(dllexport) \
void GE_Register_GlobalData(const GlobalLinkStream& stream) \
{\
	GlobalLinkData::Init(stream);\
}\


#define GE_REGISTER_SCENECLASS(ClassName) \
class ClassName; \
extern "C" __declspec(dllexport) \
GE::Scene* GE_Create_SceneClass() \
{\
    return (GE::Scene*)GE::GraphicsEngine::CreateRObject<ClassName>(); \
}\

#define GE_REGISTER_SCREENCLASS(ClassName) \
class ClassName; \
extern "C" __declspec(dllexport) \
GE::Screen* GE_Create_ScreenClass() \
{\
    return (GE::Screen*)GE::GraphicsEngine::CreateRObject<ClassName>(); \
}\


#define GE_REGISTER_DEVICECLASS(ClassName) \
class ClassName; \
extern "C" __declspec(dllexport) \
GE::GraphicsDevice* GE_Create_DeviceClass() \
{\
    return (GE::GraphicsDevice*)GE::GraphicsEngine::CreateRObject<ClassName>(); \
}\



