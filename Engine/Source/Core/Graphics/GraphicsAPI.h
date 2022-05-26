#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"

namespace JG
{
	/* 넣어야할 설정 값
	Renderer2D Shader

	
	*/

	enum class EShaderFlags;
	enum class EDepthStencilStateTemplate;
	enum class EBlendStateTemplate;
	enum class ERasterizerStateTemplate;
	enum class EResourceState;

	class InputLayout;
	class ITexture;
	class IVertexBuffer;
	class IIndexBuffer;
	class IByteAddressBuffer;
	class IStructuredBuffer;
	class IShader;
	class IGraphicsShader;
	class IComputeShader;
	class IClosestHitShader;
	class IShaderScript;
	class IFrameBuffer;
	class IMaterial;
	class IMesh;
	class IRootSignature;
	class IRootSignatureCreater;
	class IReadBackBuffer;
	class ISubMesh;
	class IRayTracingPipeline;
	class IBottomLevelAccelerationStructure;
	class ITopLevelAccelerationStructure;
	class IRayTracingShaderResourceTable;
	class RayTracer;

	struct TextureInfo;
	struct FrameBufferInfo;

	class TextureAssetStock;
	class IResource;

	namespace Graphics
	{
		class RenderPassData;
		class Light;
		class LightGrid;
	}

	class IGraphicsContext;
	class IComputeContext;
	class ICopyContext;
	class IGraphicsAPI
	{
	public:
		virtual ~IGraphicsAPI() = default;
	public:
		virtual EGraphicsAPI GetAPI() const = 0;
		virtual u64 GetBufferCount() const = 0;
		virtual u64 GetBufferIndex() const = 0;
	protected:
		friend class Application;
		friend class JGGraphics;
		virtual bool Create() = 0;
		virtual void Destroy() = 0;
	
	public:
		virtual void BeginFrame() = 0;
		virtual void EndFrame()	 = 0;
		virtual bool IsSupportedRayTracing() const = 0;
		virtual void SubmitAndFlush() = 0;
		virtual void Flush() = 0;
		virtual void SubmitAndFlush(u64 queueID) = 0;
		virtual void Flush(u64 queueID) = 0;

		virtual SharedPtr<IFrameBuffer>   CreateFrameBuffer(const FrameBufferInfo& settings) = 0;
		virtual SharedPtr<IVertexBuffer>  CreateVertexBuffer(const String& name, EBufferLoadMethod method) = 0;
		virtual SharedPtr<IIndexBuffer>   CreateIndexBuffer(const String& name, EBufferLoadMethod method) = 0;
		virtual SharedPtr<IStructuredBuffer>  CreateStrucuredBuffer(const String& name, u64 elementSize, u64 elementCount, EBufferLoadMethod method)   = 0;
		virtual SharedPtr<IByteAddressBuffer> CreateByteAddressBuffer(const String& name, u64 elementCount) = 0;
		virtual SharedPtr<IReadBackBuffer>  CreateReadBackBuffer(const String& name, u64 dataSize) = 0;
		virtual SharedPtr<IGraphicsShader> CreateGraphicsShader(const String& name, const String& sourceCode, EShaderFlags flags, const List<SharedPtr<IShaderScript>>& scriptList) = 0;
		virtual SharedPtr<IComputeShader>  CreateComputeShader(const String& name, const String& sourceCode) = 0;
		virtual SharedPtr<IClosestHitShader> CreateClosestHitShader(const String& name, const String& sourceCode, const SharedPtr<IShaderScript> script) = 0;
		virtual SharedPtr<IMaterial>	  CreateMaterial(const String& name) = 0;
		virtual SharedPtr<IMaterial>	  CreateMaterial(const String& name, SharedPtr<IGraphicsShader> shader) = 0;
		virtual SharedPtr<IMesh>          CreateMesh(const String& name) = 0;
		virtual SharedPtr<ISubMesh>       CreateSubMesh(const String& name) = 0;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name) = 0;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name, const TextureInfo& info) = 0;
		virtual SharedPtr<IRootSignatureCreater> CreateRootSignatureCreater() = 0;

		virtual SharedPtr<IRayTracingPipeline> CreateRayTracingPipeline() = 0;
		virtual SharedPtr<IRayTracingShaderResourceTable> CreateRayTracingShaderResourceTable() = 0;
		virtual SharedPtr<ITopLevelAccelerationStructure> CreateTopLevelAccelerationStructure() = 0;
		virtual SharedPtr<IBottomLevelAccelerationStructure> CreateBottomLevelAccelerationStructure() = 0;

		virtual bool AllocateCommandQueue(ECommandQueueType type, u64 queueID) = 0;
		virtual SharedPtr<IComputeContext>  GetComputeContext(u64 queueID, u64 contextID)  = 0;
		virtual SharedPtr<IGraphicsContext> GetGraphicsContext(u64 queueID, u64 contextID) = 0;
	public:
		static UniquePtr<IGraphicsAPI> Create(EGraphicsAPI api);
	};


	class IGraphicsContext
	{
	public:
		virtual ~IGraphicsContext() = default;
	public:
		// 초기 셋팅 함수
		virtual void ClearRenderTarget(const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture) = 0;
		virtual void SetRenderTarget(const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture) = 0;
		virtual void SetViewports(const List<Viewport>& viewPorts) = 0;
		virtual void SetScissorRects(const List<ScissorRect>& scissorRects) = 0;

		// Bind 함수
		virtual void BindRootSignature(SharedPtr<IRootSignature> rootSig) = 0;
		virtual void BindShader(SharedPtr<IGraphicsShader> shader) = 0;
		virtual void BindTextures(u32 rootParam, const List<SharedPtr<ITexture>>& textures) = 0;
		virtual void BindConstantBuffer(u32 rootParam, const void* data, u32 dataSize) = 0;
		virtual void BindSturcturedBuffer(u32 rootParam, SharedPtr<IResource> resource) = 0;
		virtual void BindSturcturedBuffer(u32 rootParam, const void* data, u32 elementSize, u32 elementCount) = 0;
		virtual void BindByteAddressBuffer(u32 rootParam, SharedPtr<IResource> resource) = 0;
		virtual void BindVertexAndIndexBuffer(SharedPtr<IVertexBuffer> vertexBuffer, SharedPtr<IIndexBuffer> indexBuffer) = 0;
		virtual void DrawIndexedAfterBindMeshAndMaterial(SharedPtr<IMesh> mesh, const List<SharedPtr<IMaterial>>& materialList) = 0;
		template<class T>
		void BindConstantBuffer(u32 rootParam, const T& data)
		{
			BindConstantBuffer(rootParam, &data, sizeof(T));
		}
		template<>
		void BindConstantBuffer(u32 rootParam, const List<jbyte>& data)
		{
			BindConstantBuffer(rootParam, data.data(), data.size());
		}
		template<class T>
		void BindSturcturedBuffer(u32 rootParam, const List<T>& datas)
		{
			BindSturcturedBuffer(rootParam, datas.data(), sizeof(T), datas.size());
		}
		// 셋팅 함수
		virtual void SetDepthStencilState(EDepthStencilStateTemplate _template) = 0;
		virtual void SetBlendState(u32 renderTargetSlot, EBlendStateTemplate _template) = 0;
		virtual void SetRasterizerState(ERasterizerStateTemplate _template) = 0;
		virtual void SetInputLayout(SharedPtr<InputLayout> inputLayout) = 0;

		// 그리기 함수
		virtual void DrawIndexed(u32 indexCount, u32 instancedCount = 1, u32 startIndexLocation = 0, u32 startVertexLocation = 0, u32 startInstanceLocation = 0) = 0;
		virtual void Draw(u32 vertexCount, u32 instanceCount = 1, u32 startVertexLocation = 0, u32 startInstanceLocation = 0) = 0;

		// 상태 변경 함수
		
		virtual void TransitionBarrier(const List<SharedPtr<IResource>>& resources, const List<EResourceState>& states) = 0;
		virtual void UAVBarrier(const List<SharedPtr<IResource>>& resources) = 0;

		// 인터페이스 변경 함수
		virtual SharedPtr<IComputeContext>  QueryInterfaceAsComputeContext() const  = 0;
		virtual SharedPtr<ICopyContext>		QueryInterfaceAsCopyContext() const = 0;
	protected:
		virtual void Reset() = 0;
		virtual void Reset(u64 queueID, u64 cmdID) = 0;
	};

	class IComputeContext
	{
	public:
		virtual ~IComputeContext() = default;
	public:
		virtual void ClearUAVUint(SharedPtr<IResource> resource) = 0;
		virtual void ClearUAVFloat(SharedPtr<IResource> resource) = 0;


		virtual void BindRootSignature(SharedPtr<IRootSignature> rootSig) = 0;
		virtual void BindShader(SharedPtr<IComputeShader> shader) = 0;
		virtual void BindTextures(u32 rootParam, const List<SharedPtr<ITexture>>& textures) = 0;
		virtual void BindConstantBuffer(u32 rootParam, const void* data, u32 dataSize) = 0;
		virtual void BindSturcturedBuffer(u32 rootParam, SharedPtr<IResource> sb) = 0;
		virtual void BindSturcturedBuffer(u32 rootParam, const void* data, u32 elementSize, u32 elementCount) = 0;
		virtual void BindByteAddressBuffer(u32 rootParam, SharedPtr<IResource> resource) = 0;
		virtual void BindAccelerationStructure(u32 rootParam, SharedPtr<ITopLevelAccelerationStructure> as) = 0;
		template<class T>
		void BindConstantBuffer(u32 rootParam, const T& data)
		{
			BindConstantBuffer(rootParam, &data, sizeof(T));
		}
		template<>
		void BindConstantBuffer(u32 rootParam, const List<jbyte>& data)
		{
			BindConstantBuffer(rootParam, (void*)data.data(), data.size());
		}
		template<class T>
		void BindSturcturedBuffer(u32 rootParam, const List<T>& datas)
		{
			BindSturcturedBuffer(rootParam, datas.data(), sizeof(T), datas.size());
		}


		virtual void Dispatch1D(u32 ThreadCountX, u32 GroupSizeX = 64) = 0;
		virtual void Dispatch2D(u32 ThreadCountX, u32 ThreadCountY, u32 GroupSizeX = 8, u32 GroupSizeY = 8) = 0;
		virtual void Dispatch3D(u32 ThreadCountX, u32 ThreadCountY, u32 ThreadCountZ, u32 GroupSizeX, u32 GroupSizeY, u32 GroupSizeZ) = 0;
		virtual void Dispatch(u32 groupX, u32 groupY, u32 groupZ) = 0;
		virtual void DispatchRay(u32 width, u32 height, u32 depth, SharedPtr<IRayTracingPipeline> pipeline, SharedPtr<IRayTracingShaderResourceTable> srt) = 0;
	

		virtual void TransitionBarrier(const List<SharedPtr<IResource>>& resources, const List<EResourceState>& states) = 0;
		virtual void UAVBarrier(const List<SharedPtr<IResource>>& resources) = 0;


		// 인터페이스 변경 함수
		virtual SharedPtr<ICopyContext>		QueryInterfaceAsCopyContext() const  = 0;
	protected:
		virtual void Reset(u64 queueID, u64 cmdID) = 0;
	};


	class ICopyContext
	{
	public:
		virtual ~ICopyContext() = default;
	public:
		virtual void CopyResource(SharedPtr<IResource> dest, SharedPtr<IResource> src) = 0;
		virtual void CopyTextureRegion(SharedPtr<IResource> dest, SharedPtr<IResource> src, const JRect& srcRect, EResourceState inDestState, EResourceState inSrcState) = 0;
		virtual void CopyBuffer(SharedPtr<IResource> sb, const void* datas, u64 elementSize, u64 elementCount) = 0;
		virtual SharedPtr<IMesh> CopyMesh(SharedPtr<IMesh> src) = 0;
	};




}















