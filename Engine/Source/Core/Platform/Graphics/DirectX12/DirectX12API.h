#pragma once
#include "Graphics/GraphicsAPI.h"
#include "Graphics/GraphicsDefine.h"
#include "Utill/DirectX12Helper.h"

enum   DXGI_FORMAT;
struct ID3D12Device5;
struct IDXGIFactory4;
struct ID3D12Resource;
struct D3D12_DEPTH_STENCIL_DESC;
struct D3D12_RENDER_TARGET_BLEND_DESC;
struct D3D12_RASTERIZER_DESC;
struct D3D12_CPU_DESCRIPTOR_HANDLE;

namespace JG
{
	class ITexture;
	class IReadBackBuffer;
	class IFrameBuffer;
	class RootSignature;
	class CommandQueue;
	class DescriptorAllocation;
	class DescriptorAllocator;
	class GraphicsCommandList;
	class ComputeCommandList;
	class CopyCommandList;
	class GraphicsPipelineState;
	class ComputePipelineState;
	class TextureAssetStock;
	class DirectX12FrameBuffer;
	class DirectX12API : public IGraphicsAPI
	{
	public:
		virtual EGraphicsAPI GetAPI()const override;
		virtual u64 GetBufferCount() const override;
		virtual u64 GetBufferIndex() const override;
	public:
		static DirectX12API*  GetInstance();
		static IDXGIFactory4* GetDXGIFactory();
		static ID3D12Device5*  GetD3DDevice();
		static CommandQueue*  GetGraphicsCommandQueue();
		static CommandQueue*  GetComputeCommandQueue();
		static CommandQueue*  GetCopyCommandQueue();
		static u64			  GetFrameBufferCount();
		static u64			  GetFrameBufferIndex();


		static DescriptorAllocation RTVAllocate();
		static DescriptorAllocation DSVAllocate();
		static DescriptorAllocation CSUAllocate();

		static GraphicsCommandList* GetGraphicsCommandList();
		static ComputeCommandList*  GetComputeCommandList();
		static CopyCommandList*     GetCopyCommandList();

		static SharedPtr<GraphicsPipelineState> GetGraphicsPipelineState();
		static SharedPtr<ComputePipelineState>  GetComputePipelineState();
		static SharedPtr<RootSignature>         GetGraphicsRootSignature();
		static SharedPtr<RootSignature>         GetComputeRootSignature();



		static void GetDepthStencilDesc(EDepthStencilStateTemplate _template,  D3D12_DEPTH_STENCIL_DESC* out);
		static void GetBlendDesc(EBlendStateTemplate _template,  D3D12_RENDER_TARGET_BLEND_DESC* out);
		static void GetRasterizerDesc(ERasterizerStateTemplate _template,  D3D12_RASTERIZER_DESC* out);


		static Microsoft::WRL::ComPtr<ID3D12Resource> CreateCommittedResource(
			const String& name,
			const D3D12_HEAP_PROPERTIES* pHeapProperties,
			D3D12_HEAP_FLAGS HeapFlags,
			const D3D12_RESOURCE_DESC* pDesc,
			D3D12_RESOURCE_STATES InitialResourceState,
			const D3D12_CLEAR_VALUE* pOptimizedClearValue);
		static void DestroyCommittedResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource);
	private:
		static SharedPtr<RootSignature> CreateGraphicsRootSignature();
		static SharedPtr<RootSignature> CreateComputeRootSignature();
		static void AddStaticSamplerState(SharedPtr<RootSignature> rootSig);
	protected:
		// Application
		virtual bool Create() override;
		virtual void Destroy() override;
		// API
		virtual void BeginFrame() override;
		virtual void EndFrame()   override;
	public:
		virtual void Flush() override;
	protected:
		virtual void BeginDraw() override;
		virtual void EndDraw()   override;

		virtual void SetRenderPassData( const Graphics::RenderPassData& passData)   override;
		virtual void SetLightGrids( SharedPtr<IStructuredBuffer> rwBuffer) override;
		virtual void SetLightGrids( const List<Graphics::LightGrid>& lightGrids) override;
		virtual void SetVisibleLightIndicies( const List<u32>& visibleLightIndicies) override;
		virtual void SetVisibleLightIndicies( const SharedPtr<IStructuredBuffer> rwBuffer) override;
		virtual void SetLights( const List<SharedPtr<Graphics::Light>>& lights) override;
		virtual void SetTextures( const List<SharedPtr<ITexture>>& textures) override;
		virtual void SetTransform( const JMatrix* worldmats, u64 instanceCount = 1) override;
		virtual void SetViewports( const List<Viewport>& viewPorts) override;
		virtual void SetScissorRects( const List<ScissorRect>& scissorRects) override;
		virtual void ClearRenderTarget( const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture) override;
		virtual void ClearUAVUint( SharedPtr<IByteAddressBuffer> buffer) override;
		virtual void SetRenderTarget( const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture) override;
		virtual void DrawIndexed(u32 indexCount, u32 instancedCount = 1, u32 startIndexLocation = 0, u32 startVertexLocation = 0, u32 startInstanceLocation = 0) override;
		virtual void Draw(u32 vertexCount, u32 instanceCount = 1, u32 startVertexLocation = 0, u32 startInstanceLocation = 0) override;
	protected:
		virtual void SetDepthStencilState( EDepthStencilStateTemplate _template) override;
		virtual void SetBlendState( u32 renderTargetSlot, EBlendStateTemplate _template) override;
		virtual void SetRasterizerState( ERasterizerStateTemplate _template) override;
	protected:
		virtual SharedPtr<IFrameBuffer>   CreateFrameBuffer(const FrameBufferInfo& info) override;
		virtual SharedPtr<IVertexBuffer>  CreateVertexBuffer(const String& name, EBufferLoadMethod method) override;
		virtual SharedPtr<IIndexBuffer>   CreateIndexBuffer(const String& name, EBufferLoadMethod method) override;

		virtual SharedPtr<IStructuredBuffer>  CreateStrucuredBuffer(const String& name, u64 elementSize, u64 elementCount) override;
		virtual SharedPtr<IByteAddressBuffer> CreateByteAddressBuffer(const String& name, u64 elementCount) override;
		virtual SharedPtr<IReadBackBuffer>  CreateReadBackBuffer(const String& name) override;
		virtual SharedPtr<IComputer>      CreateComputer(const String& name, SharedPtr<IComputeShader> shader) override;
		virtual SharedPtr<IGraphicsShader> CreateGraphicsShader(const String& name, const String& sourceCode, EShaderFlags flags, const List<SharedPtr<IShaderScript>>& scriptList) override;
		virtual SharedPtr<IComputeShader>  CreateComputeShader(const String& name, const String& sourceCode) override;
		virtual SharedPtr<IMaterial>	  CreateMaterial(const String& name) override;
		virtual SharedPtr<IMaterial>	  CreateMaterial(const String& name, SharedPtr<IGraphicsShader> shader) override;
		virtual SharedPtr<IMesh>		  CreateMesh(const String& name) override;
		virtual SharedPtr<ISubMesh>       CreateSubMesh(const String& name) override;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name) override;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name, const TextureInfo& info) override;


		virtual SharedPtr<IComputeContext> GetComputeContext()   const override;
		virtual SharedPtr<IGraphicsContext> GetGraphicsContext() const override;
	private:
		// юс╫ц 
	
		static DirectX12API* sm_DirectX12API;
		

		ComPtr<IDXGIFactory4> mFactory;
		ComPtr<ID3D12Device5>  mDevice;
		SharedPtr<DescriptorAllocator> mCSUAllocator;
		SharedPtr<DescriptorAllocator> mRTVAllocator;
		SharedPtr<DescriptorAllocator> mDSVAllocator;

		SharedPtr<CommandQueue> mGraphicsCommandQueue;
		SharedPtr<CommandQueue> mComputeCommandQueue;
		SharedPtr<CommandQueue> mCopyCommandQueue;


		Dictionary<handle, SharedPtr<DirectX12FrameBuffer>> mFrameBuffers;
		const u64 mFrameBufferCount = 3;
		u64       mFrameBufferIndex = 0;

		std::mutex mGraphicsPSOMutex;
		std::mutex mComputePSOMutex;
		std::mutex mRootSigMutex;


		Dictionary<std::thread::id, SharedPtr<GraphicsPipelineState>> mGraphicsPSOs;
		Dictionary<std::thread::id, SharedPtr<ComputePipelineState>>  mComputePSOs;
		Dictionary<std::thread::id, SharedPtr<RootSignature>> mGraphicsRootSignatures;
		Dictionary<std::thread::id, SharedPtr<RootSignature>> mComputeRootSignatures;

		std::mutex mDeviceMutex;

	};


	class DirectX12GraphicsContext : public IGraphicsContext
	{



	};


	class DirectX12ComputeContext : public IComputeContext
	{

	};







}
 