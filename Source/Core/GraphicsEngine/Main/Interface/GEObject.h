#pragma once


#ifdef GRAPHICSENGINE_EXPORT

#define GRAPHICSENGINE_API __declspec(dllexport)

#else

#define GRAPHICSENGINE_API __declspec(dllimport)

#endif

#include "CommonCore.h"
#include "ShaderDefined.h"
#include "StateWrapper.h"


#define GELOG(level, contents, ...) JGLOG(level, GraphicsEngine, contents,  __VA_ARGS__)

#define GELOG_DEBUG(contents, ...) GELOG(LogLevel::Debug, contents, __VA_ARGS__)
#define GELOG_INFO(contents, ...)  GELOG(LogLevel::Info, contents, __VA_ARGS__)
#define GELOG_WARN(contents, ...)  GELOG(LogLevel::Warning, contents, __VA_ARGS__)
#define GELOG_ERROR(contents, ...) GELOG(LogLevel::Error, contents, __VA_ARGS__)
#define GELOG_FATAL(contents, ...) GELOG(LogLevel::Fatal, contents, __VA_ARGS__)

namespace GE
{

	class GRAPHICSENGINE_API GEObject
	{
		
	public:
		template<class T>
		static T* CreateGEObject()
		{
			return new T();
		}


		template<class T>
		using LoadFunc = T* (*)();

	};


	template<class T>
	class GRAPHICSENGINE_API GEObjectRef
	{
	private:
	
		GEObjectRef(const GEObjectRef& copy) = delete;
		GEObjectRef& operator=(const GEObjectRef& rhs) = delete;
	public:
		GEObjectRef() = default;
		GEObjectRef(T* ptr, class GraphicsEngine* engine, bool is_auto_delete = true) {
			m_Ptr = ptr;
			m_GraphicsEngine = engine;
			m_IsAutoDelete = is_auto_delete;
			
		}
		GEObjectRef(GEObjectRef&& rhs) noexcept {
			m_Ptr = rhs.m_Ptr;
			m_GraphicsEngine = rhs.m_GraphicsEngine;
			m_IsAutoDelete   = rhs.m_IsAutoDelete;

			rhs.m_Ptr = nullptr;
			rhs.m_GraphicsEngine = nullptr;
			rhs.m_IsAutoDelete = true;
		}
		GEObjectRef& operator=(GEObjectRef&& rhs) {
			Reset();

			m_Ptr = rhs.m_Ptr;
			m_GraphicsEngine = rhs.m_GraphicsEngine;
			m_IsAutoDelete   = rhs.m_IsAutoDelete;


			rhs.m_Ptr = nullptr;
			rhs.m_GraphicsEngine = nullptr;
			rhs.m_IsAutoDelete   = true;
			return *this;
		}


		T* operator->() const {
			return m_Ptr;
		}


		bool IsValid() const {
			return m_Ptr != nullptr;
		}
		T* Get() const {
			return m_Ptr;
		}
		void Reset() {
			if (m_Ptr && m_IsAutoDelete)
			{
				delete m_Ptr;
				m_Ptr = nullptr;
			}
		}
		~GEObjectRef()
		{
			Reset();
		}
	private:
		T* m_Ptr = nullptr;
		class GraphicsEngine* m_GraphicsEngine = nullptr;
		bool m_IsAutoDelete = true;
	};
}










#define REGISTER_GRAPHICS_INTERFACE_(interface_name, className) \
class className;\
extern "C" __declspec(dllexport) \
inline className* Load_GraphicsInterface_##interface_name() \
{ \
	return GE::GEObject::CreateGEObject<##className>(); \
} \



#define REGISTER_GRAPHICS_INTERFACE_GRAPHICSDEVICE(className)   REGISTER_GRAPHICS_INTERFACE_(GraphicsDevice, className)
#define REGISTER_GRAPHICS_INTERFACE_GRAPHICSRENDERER(className) REGISTER_GRAPHICS_INTERFACE_(GraphicsRenderer, className)
#define REGISTER_GRAPHICS_INTERFACE_SCENE(className)			REGISTER_GRAPHICS_INTERFACE_(Scene, className)
#define REGISTER_GRAPHICS_INTERFACE_SCENEOBJECT(className)		REGISTER_GRAPHICS_INTERFACE_(SceneObject, className)
#define REGISTER_GRAPHICS_INTERFACE_SCREEN(className)			REGISTER_GRAPHICS_INTERFACE_(Screen, className)
#define REGISTER_GRAPHICS_INTERFACE_MATERIAL(className)			REGISTER_GRAPHICS_INTERFACE_(Material, className)
#define REGISTER_GRAPHICS_INTERFACE_TEXTURE(className)			REGISTER_GRAPHICS_INTERFACE_(Texture, className)


#define LOAD_GRAPHICS_INTERFACE(plugin, interface_name) ((GE::GEObject::LoadFunc<GE::##interface_name>)plugin->GetProcAddress(std::string("Load_GraphicsInterface_") + #interface_name))()
