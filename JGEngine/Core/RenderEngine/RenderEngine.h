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
	class RENDERENGINE_API RenderEngine : public EngineCore
	{
	private:
		std::shared_ptr<RenderDevice>      m_RenderDevice;
		std::shared_ptr<ShaderLibManager>  m_ShaderLibManager;
		std::shared_ptr<RenderItemManager> m_RenderItemManager;
		std::shared_ptr<ShaderModuleManager> m_ShaderModuleManager;


		std::shared_ptr<RenderTarget> m_RenderTarget;
		RE_EventListener m_EventListener;
		std::vector<Resource> m_UploadResource;


		std::shared_ptr<ReCamera> m_Cam;
	public:
		RenderEngine(const GlobalLinkStream& stream);
		virtual ~RenderEngine();
	public:
		void Init(HWND hWnd, int width, int height, const std::shared_ptr<GUI>& bind_gui);
	public:
		virtual void Load() override;
		virtual void Update() override;
		virtual void OnEvent(Event& e) override;
	private:
		void LoadTexture(const std::wstring& name, CommandList* cmdList);
	};
}
