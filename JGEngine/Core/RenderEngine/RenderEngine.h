#pragma once
#include "RE.h"
#include "DataType/Abstract/EngineCore.h"
#include "RE_EventListener.h"
#include "CommonCore.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <DirectXMath.h>
namespace RE
{
	class Fence;
	class RenderDevice;
	class Resource;
	class Texture;
	class GUI;
	class GraphicsShaderModule;
	class ComputeShaderModule;
	class GUIModule;
	class ShaderLibManager;
	class RenderItemManager;
	class ShaderModuleManager;
	class DxScreen;
	class TextureManager;
	class ReMaterialManager;
	class ReMaterial;
	class RenderItem;
	enum class ERenderItemUsage;
	class RENDERENGINE_API RenderEngineConfig
	{
	public:
		std::string ShaderLibPath    = "Shader\\Lib\\ShaderTypeLib.jg";
		std::string ShaderModulePath = "Shader\\Module";
		std::string TexturePath      = "Textures";
		std::string StaticEntryModuleName    = "StaticGBuffer";
		std::string SkeletalEntryModuleName  = "None";
		std::string GUIEntryModuleName       = "JGUI";

		//


	};

#define RE_GUI_DefaultMaterial      "M_DefaultGUI"
#define RE_GUI_OneTextureDefault    "M_DefaultTextureGUI"
#define RE_GUI_TextMaterial         "M_DefaultGUIText"
	enum RENDERENGINE_API ERE_GUI_Material
	{
		GUI_Default,
		GUI_LineDefault
	};

	class RENDERENGINE_API RenderEngine : public EngineCore
	{
	public:
		// Window 관련

		static DxScreen* CreateDxScreen(HWND hWnd, uint32_t width, uint32_t height,
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
		static void      DestroyDxScreen(HWND hWnd);
		static void      DestroyDxScreen(DxScreen* screen);
		static DxScreen* FindDxScreen(HWND hWnd);


		// RenderItem 관련
		static void        RegisterRIManager(uint64_t id);
		static RenderItem* CreateRenderItem(uint64_t id, ERenderItemUsage usage, const std::string& name = "RenderItem");
		static void        DestroyRenderItem(uint64_t id, RenderItem* item);
		static void        UnRegisterRIManager(uint64_t id);

		// 아직 미구현
		static void        MoveRenderItem(uint64_t old_id, uint64_t new_id);


		// Shader Module 관련

		static GraphicsShaderModule* FindGraphicsShaderModule(const std::string& name);
		static ComputeShaderModule*  FindComputeShaderModule(const std::string& name);
		static GUIModule* GetGUIModule();


		// Material 관련
		void        RegisterMaterial(const std::string& name, const ReMaterial& m);
		ReMaterial* FindMaterial(const std::string& name);


		// 기타
		static RenderDevice* GetDevice();
		// TestCode
		static void SaveToFile(const std::string& path, const Texture& t);
		static void SaveToFile(const std::string& path, const std::string& texturename);
		//
		static void RequestTextureLoad(const std::string& path);
		static void RegisterTexture(const std::string& name, const Texture& t);
		static void UnRegisterTexture(const std::string& name);
	public:
		RenderEngine(const GlobalLinkStream& stream);
		virtual ~RenderEngine();
	public:
		void Init(const RenderEngineConfig& config = RenderEngineConfig());
	public:
		virtual void Load() override;
		virtual void Update() override;
		virtual void OnEvent(Event& e) override;
	private:
		std::shared_ptr<RenderDevice>        m_RenderDevice;
		std::shared_ptr<ShaderLibManager>    m_ShaderLibManager;
		std::shared_ptr<ShaderModuleManager> m_ShaderModuleManager;
		std::shared_ptr<TextureManager>      m_TextureManager;
		std::shared_ptr<ReMaterialManager>   m_MaterialManager;

		std::unordered_map<uint64_t, std::shared_ptr<RenderItemManager>> m_RIManager;
		RenderEngineConfig m_Config;
	};


}
