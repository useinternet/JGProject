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
	class CommandList;
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
	protected:
		// Application
		virtual bool Create() override;
		virtual void Destroy() override;

		// API
		virtual void BeginFrame() override;
		virtual void EndFrame()   override;
	public:
		virtual bool IsSupportedRayTracing() const override;
		virtual void Flush() override;
	protected:
		virtual SharedPtr<IFrameBuffer>   CreateFrameBuffer(const FrameBufferInfo& info) override;
		virtual SharedPtr<IVertexBuffer>  CreateVertexBuffer(const String& name, EBufferLoadMethod method) override;
		virtual SharedPtr<IIndexBuffer>   CreateIndexBuffer(const String& name, EBufferLoadMethod method) override;

		virtual SharedPtr<IStructuredBuffer>  CreateStrucuredBuffer(const String& name, u64 elementSize, u64 elementCount) override;
		virtual SharedPtr<IByteAddressBuffer> CreateByteAddressBuffer(const String& name, u64 elementCount) override;
		virtual SharedPtr<IReadBackBuffer>  CreateReadBackBuffer(const String& name) override;
		virtual SharedPtr<IGraphicsShader> CreateGraphicsShader(const String& name, const String& sourceCode, EShaderFlags flags, const List<SharedPtr<IShaderScript>>& scriptList) override;
		virtual SharedPtr<IComputeShader>  CreateComputeShader(const String& name, const String& sourceCode) override;
		virtual SharedPtr<IMaterial>	  CreateMaterial(const String& name) override;
		virtual SharedPtr<IMaterial>	  CreateMaterial(const String& name, SharedPtr<IGraphicsShader> shader) override;
		virtual SharedPtr<IMesh>		  CreateMesh(const String& name) override;
		virtual SharedPtr<ISubMesh>       CreateSubMesh(const String& name) override;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name) override;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name, const TextureInfo& info) override;
		virtual SharedPtr<IRootSignatureCreater> CreateRootSignatureCreater() override;
		virtual SharedPtr<IRayTracingPipeline> CreateRayTracingPipeline() override;
		virtual SharedPtr<ITopLevelAccelerationStructure> CreateTopLevelAccelerationStructure() override;
		virtual SharedPtr<IBottomLevelAccelerationStructure> CreateBottomLevelAccelerationStructure() override;


		virtual SharedPtr<IGraphicsContext> GetGraphicsContext() override;
		virtual SharedPtr<IComputeContext>  GetComputeContext()  override;
	private:
		// 임시 
	
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
		std::mutex mGraphicsContextMutex;
		std::mutex mComputeContextMutex;


		Dictionary<std::thread::id, SharedPtr<GraphicsPipelineState>> mGraphicsPSOs;
		Dictionary<std::thread::id, SharedPtr<ComputePipelineState>>  mComputePSOs;
		Dictionary<std::thread::id, SharedPtr<IGraphicsContext>> mGraphicsContextDic;
		Dictionary<std::thread::id, SharedPtr<IComputeContext>> mComputeContextDic;

		std::mutex mDeviceMutex;


		bool mIsSupportedRayTracing = false;
		
	};


	class DirectX12GraphicsContext : public IGraphicsContext
	{
		GraphicsCommandList*      mCommandList         = nullptr;
		SharedPtr<IRootSignature> mBindedRootSignature = nullptr;

		mutable SharedPtr<ComputeCommandList> mCacheComputeCommandList = nullptr;
	public:
		// 초기 셋팅 함수
		virtual void ClearRenderTarget(const List<SharedPtr<ITexture>>&rtTextures, SharedPtr<ITexture> depthTexture) override;
		virtual void SetRenderTarget(const List<SharedPtr<ITexture>>&rtTextures, SharedPtr<ITexture> depthTexture) override;
		virtual void SetViewports(const List<Viewport>&viewPorts) override;
		virtual void SetScissorRects(const List<ScissorRect>&scissorRects) override;

		// Bind 함수
		virtual void BindRootSignature(SharedPtr<IRootSignature> rootSig) override;
		virtual void BindShader(SharedPtr<IGraphicsShader> shader) override;
		virtual void BindTextures(u32 rootParam, const List<SharedPtr<ITexture>>&textures) override;
		virtual void BindConstantBuffer(u32 rootParam, const void*, u32 dataSize) override;
		virtual void BindSturcturedBuffer(u32 rootParam, SharedPtr<IStructuredBuffer> sb) override;
		virtual void BindSturcturedBuffer(u32 rootParam, const void* data, u32 elementSize, u32 elementCount) override;
		virtual void BindByteAddressBuffer(u32 rootParam, SharedPtr<IByteAddressBuffer> bab) override;
		virtual void BindVertexAndIndexBuffer(SharedPtr<IVertexBuffer> vertexBuffer, SharedPtr<IIndexBuffer> indexBuffer) override;
		virtual void DrawIndexedAfterBindMeshAndMaterial(SharedPtr<IMesh> mesh, const List<SharedPtr<IMaterial>>& materialList) override;
		// 셋팅 함수
		virtual void SetDepthStencilState(EDepthStencilStateTemplate _template) override;
		virtual void SetBlendState(u32 renderTargetSlot, EBlendStateTemplate _template) override;
		virtual void SetRasterizerState(ERasterizerStateTemplate _template) override;
		virtual void SetInputLayout(SharedPtr<InputLayout> inputLayout) override;

		// 그리기 함수
		virtual void DrawIndexed(u32 indexCount, u32 instancedCount, u32 startIndexLocation, u32 startVertexLocation, u32 startInstanceLocation) override;
		virtual void Draw(u32 vertexCount, u32 instanceCount, u32 startVertexLocation, u32 startInstanceLocation) override;

		// 인터페이스 변경 함수
		virtual SharedPtr<IComputeContext> QueryInterfaceAsComputeContext() const override;
		virtual SharedPtr<ICopyContext>	   QueryInterfaceAsCopyContext() const override;
	public:
		virtual void Reset() override;
	};


	class DirectX12ComputeContext : public IComputeContext
	{
		friend DirectX12GraphicsContext;
	public:
		ComputeCommandList*       mCommandList = nullptr;
		SharedPtr<IRootSignature> mBindedRootSignature = nullptr;
	public:
		// Clear 함수
		virtual void ClearUAVUint(SharedPtr<IByteAddressBuffer> buffer) override;
		virtual void ClearUAVFloat(SharedPtr<IByteAddressBuffer> buffer) override;

		// Bind 함수
		virtual void BindRootSignature(SharedPtr<IRootSignature> rootSig) override;
		virtual void BindShader(SharedPtr<IComputeShader> shader) override;
		virtual void BindTextures(u32 rootParam, const List<SharedPtr<ITexture>>& textures) override;


		virtual void BindConstantBuffer(u32 rootParam, const void* data, u32 dataSize) override;
		virtual void BindSturcturedBuffer(u32 rootParam, SharedPtr<IStructuredBuffer> sb) override;
		virtual void BindSturcturedBuffer(u32 rootParam, const void* data, u32 elementSize, u32 elementCount) override;
		virtual void BindByteAddressBuffer(u32 rootParam, SharedPtr<IByteAddressBuffer> bab) override;
		virtual void BindAccelerationStructure(u32 rootParam, SharedPtr<ITopLevelAccelerationStructure> as) override;
		// Dispatch 함수
		virtual void Dispatch1D(u32 ThreadCountX, u32 GroupSizeX = 64) override;
		virtual void Dispatch2D(u32 ThreadCountX, u32 ThreadCountY, u32 GroupSizeX = 8, u32 GroupSizeY = 8) override;
		virtual void Dispatch3D(u32 ThreadCountX, u32 ThreadCountY, u32 ThreadCountZ, u32 GroupSizeX, u32 GroupSizeY, u32 GroupSizeZ) override;
		virtual void Dispatch(u32 groupX, u32 groupY, u32 groupZ) override;
		virtual void DispatchRay(u32 width, u32 height, u32 depth, SharedPtr<IRayTracingPipeline> pipeline) override;
		virtual void Reset() override;


		virtual SharedPtr<ICopyContext>		QueryInterfaceAsCopyContext() const override;
	public:
		ComputeCommandList* Get() const {
			return mCommandList;
		}
	};



	class DirectX12CopyContext : public ICopyContext
	{
		friend DirectX12GraphicsContext;
		friend DirectX12ComputeContext;
		CommandList* mCommandList = nullptr;
	public:
		virtual void CopyBuffer(SharedPtr<IStructuredBuffer> sb, const void* datas, u64 elementSize, u64 elementCount) override;
	};



}
 