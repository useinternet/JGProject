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
	class CommandQueue;
	class CommandList;
	class DescriptorAllocator;
	class DescriptorHandle;
	class Resource;
	class ResourceData;
	class Texture;
	class GUI;
	class RenderTarget;
	class RootSignature;
	class PipelineState;
	class GraphicsPipelineState;
	class VertexShader;
	class PixelShader;
	class GraphicsShaderModule;
	class ShaderLibManager;
	class RenderItemManager;
	class ShaderModuleManager;
	class ReCamera;
	class DxScreen;
	class TextureManager;

	class RENDERENGINE_API RenderEngineConfig
	{
	public:
		std::string ShaderLibPath;
		std::string ShaderModulePath;
		std::string TexturePath;
		std::string MatStaticEntryModuleName;
		std::string MatSkeletalEntryModuleName;
		std::string MatGUIEntryModuleName;

		//


	};

	class RENDERENGINE_API RenderEngine : public EngineCore
	{
	private:
		std::shared_ptr<RenderDevice>      m_RenderDevice;
		std::shared_ptr<ShaderLibManager>  m_ShaderLibManager;
		std::shared_ptr<RenderItemManager> m_RenderItemManager;
		std::shared_ptr<ShaderModuleManager> m_ShaderModuleManager;
		std::shared_ptr<TextureManager>    m_TextureManager;

		std::shared_ptr<RenderTarget> m_RenderTarget;
		std::vector<Resource>         m_UploadResource;


		std::shared_ptr<ReCamera> m_Cam;
		DxScreen* m_MainScreen = nullptr;
	public:
		RenderEngine(const GlobalLinkStream& stream);
		virtual ~RenderEngine();
	public:
		void Init(HWND hWnd, int width, int height);
	public:
		virtual void Load() override;
		virtual void Update() override;
		virtual void OnEvent(Event& e) override;
	public:
		static DxScreen* CreateDxScreen(HWND hWnd, uint32_t width, uint32_t height, 
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
		static void DestroyDxScreen(HWND hWnd);
		static void DestroyDxScreen(DxScreen* screen);
		static DxScreen* FindDxScreen(HWND hWnd);
	};


}
