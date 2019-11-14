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

	
	struct Vertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 Tangent;
		DirectX::XMFLOAT2 Tex;
		Vertex() {}
		Vertex(float p1, float p2, float p3, 
			float n1, float n2, float n3,
			float t1, float t2, float t3,
			float u, float v) :
			Pos({ p1, p2, p3 }), 
			Normal({ n1,n2,n3 }),
			Tangent({t1,t2,t3}),
			Tex({ u, v }) {}
	};
	struct ObjectConstants
	{
		JMatrix World = JMatrix::Identity();
		JMatrix ViewProj;
		uint32_t Frame = 0;
		JVector3 padding;
	};

	struct RenderItem
	{
		std::shared_ptr<GraphicsPipelineState> PSO;
		std::shared_ptr<RootSignature> Rootsignature;
		std::shared_ptr<VertexShader> VertexShader;
		std::shared_ptr<PixelShader> PixelShader;
		std::vector<Vertex> vertices;
		std::vector<std::uint32_t> indices;
		ObjectConstants contants1;
		ObjectConstants contants2;
		ObjectConstants contants3;
		int width, height;

		std::shared_ptr<RenderTarget> rendertarget;
		std::vector<Texture> textures;
	};

	class GraphicsShaderModule;
	class ShaderLibManager;
	class RENDERENGINE_API RenderEngine : public EngineCore
	{
	private:
		std::shared_ptr<RenderDevice>     m_RenderDevice;
		std::shared_ptr<ShaderLibManager> m_ShaderLibManager;







		RE_EventListener m_EventListener;
		RenderItem m_RenderItem;
		std::vector<Resource> m_UploadResource;
		std::shared_ptr<GraphicsShaderModule> m_Module;
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
		void CreateBox(float  width, float height, float depth);
		void LoadTexture(const std::wstring& name, CommandList* cmdList);
	};
}
