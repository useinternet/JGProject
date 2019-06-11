#pragma once
#include"Dx12Include.h"
#include"GraphicsResource.h"
#include<atomic>
#include<vector>
#include<string>
namespace GR
{
	namespace Dx12
	{
		class Fence;
		class DescriptorAllocator;
		class CommandExecutorManager;
		class Commander;
		class GraphicsCommander;
		class ComputeCommander;
		class CopyCommander;
		class Renderer;
		class Vertex;
		class Mesh;
		class PSOCache;
		class RootSignatureCache;
		class GraphicsPSO;
		class ComputePSO;
		class RootSignature;
		class GPUAllocator;
		class GPUAllocation;
		enum class ERootSignature;
		class GraphicsDevice 
		{
			using UIInitEvent = std::function<void(HWND, GraphicsDevice*)>;
			using UIDestroyEvent = std::function<void()>;
			using UINewFrameEvent = std::function<void()>;
			using UIRenderEvent = std::function<void(GraphicsCommander*)>;
			using ShaderCache = std::unordered_map<uint32_t, ComPtr<ID3DBlob>>;
		public:
			struct UIEvents
			{
				UIInitEvent     initEvent;
				UIDestroyEvent  destroyEvent;
				UINewFrameEvent newFrameEvent;
				UIRenderEvent   renderEvent;
			};
			// 
		public:
			// 리소스 힙 타입
			enum EHeapType
			{
				DEFAULT,
				UPLOAD,
				READBACK,
				CUSTOM
			};
			// Descriptor 힙 타입
			enum EDescritporHeapType
			{
				SAMPLER,
				SRV_UAV_CBV,
				RTV,
				DSV,
				DH_NUM
			};
			// 커맨드 리스트 타입
			enum ECmdQueueType
			{
				GRAPHICS,
				COMPUTE,
				COPY,
				CQ_NUM
			};
			//
			enum ELoadCommanderType
			{
				TEXTURE_LOADER,
				MESH_LOADER,
				NUM_LOADER
			};
			static GraphicsDevice* GetPointer() { return ms_App; }
		private:
			// 엔진 설정들
			static const uint32_t    ms_FrameCount = 3;
			static const uint32_t    ms_UseVsync = 0;
			static const DXGI_FORMAT ms_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			//
			static GraphicsDevice* ms_App;
			static uint64_t ms_EngineFrame;
			uint32_t m_FrameIndex;
			std::string m_EngineShaderEntitys[Shader::NUM_SHADERSTAGE];
			std::string m_EngineShaderVersions[Shader::NUM_SHADERSTAGE];
			ShaderCache m_ShaderCache[Shader::NUM_SHADERSTAGE];
			//
			ComPtr<ID3D12Device>    m_Device;
			ComPtr<IDXGIFactory4>   m_Factory;
			ComPtr<IDXGISwapChain4> m_SwapChain;
			std::shared_ptr<DescriptorAllocator>    m_DescriptorAllocator[DH_NUM];
			std::shared_ptr<GPUAllocator> m_UIGpuAllocator;
		public:
			std::shared_ptr<CommandExecutorManager> m_CommandExecutorManager;
			std::shared_ptr<Renderer>               m_GraphicsRenderer;
			std::shared_ptr<PSOCache>               m_PSOCache;
			std::shared_ptr<RootSignatureCache>     m_RootSigCache;
			//
			// RenderTarget 소스
			ColorTexture m_BackBufferResource[ms_FrameCount];
			//
			//
			uint32_t m_Width, m_Height;
			HWND     m_hWnd;
			uint32_t m_Increment_Sampler_DescriptorSize;
			uint32_t m_Increment_SrvUavCbv_DescriptorSize;
			uint32_t m_Increment_Rtv_DescriptorSize;
			uint32_t m_Increment_Dsv_DescriptorSize;

			uint64_t m_FenceValue[ms_FrameCount];
			uint64_t m_FrameValue[ms_FrameCount];
	
			// UI 이벤트
			UIInitEvent     m_InitUIEvent;
			UIDestroyEvent  m_DestroyUIEvent;
			UINewFrameEvent m_NewFrameUIEvent;
			UIRenderEvent   m_DrawUIEvent;
			// 커맨더 자원관리 
			std::vector<Commander*> m_WaitingCommanders[CQ_NUM];
			GraphicsCommander*      m_LoadCommander[NUM_LOADER];
			std::mutex              m_PushCommanderMutex;
		public:
			GraphicsDevice();
			virtual ~GraphicsDevice();

			virtual void Initialize(HWND hWnd, uint32_t width, uint32_t height,bool isUseWrap, UIEvents* e = nullptr);
			virtual void NewFrame();
			virtual void Resize(uint32_t width, uint32_t height);
			virtual void Present(ColorTexture* texture = nullptr);
			virtual void Flush();
			virtual void Flush(ECmdQueueType type);
		public:
			DescriptorAllocation Allocate(EDescritporHeapType type, uint32_t numDescriptor = 1);
			void ExecuteCommander(ECmdQueueType type, bool is_flush);
			void ExecuteCommander(bool is_flush);
			void ExecuteCommanderWithFenceValue(ECmdQueueType type, uint64_t* outValue);
			void PushCommander(GraphicsCommander* commander);
			void PushCommander(ComputeCommander* commander);
			void PushCommander(CopyCommander* commander);
			GraphicsCommander* GetGraphicsCommander();
			ComputeCommander*  GetComputeCommander();
			CopyCommander*     GetCopyCommander();
			GraphicsCommander* GetLoadCommander(ELoadCommanderType type);
			Renderer*   GetRenderer();
			GraphicsPSO GetGraphicsPSOFromCache(RootSignature& rootSig, uint32_t enumPso, uint32_t macrooption = 0);;
			ComputePSO  GetComputePSOFromCache(RootSignature& rootSig, uint32_t enumPso, uint32_t macrooption = 0);
			RootSignature GetRootSignatureFromCache(ERootSignature enumRootSig);
			GPUAllocation UIGPUAllcoate();
			GPUAllocation UIGPUAllocateAndRegister(Texture* in_texture);
			void SetShaderDirPath(const std::wstring& path);
		public:
			GPUResource CreateGPUResource(
				const D3D12_RESOURCE_DESC& desc,
				D3D12_CLEAR_VALUE* clearValue = nullptr,
				EHeapType type = DEFAULT,
				D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON, 
				D3D12_HEAP_FLAGS flag       = D3D12_HEAP_FLAG_NONE);
			GPUResource CreateGPUResource(
				const D3D12_RESOURCE_DESC& desc,
				const float4& clearColor,
				EHeapType type = DEFAULT,
				D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON,
				D3D12_HEAP_FLAGS flag = D3D12_HEAP_FLAG_NONE) {
				float color[4];
				color[0] = clearColor.x; 		color[1] = clearColor.y; 		color[2] = clearColor.z;
				color[3] = clearColor.w;
				return CreateGPUResource(desc, &CD3DX12_CLEAR_VALUE(desc.Format, color), type, state, flag);
			}
			GPUResource CreateGPUResource(
				const D3D12_RESOURCE_DESC& desc,
				float cleardepth, uint8_t clearstencil,
				EHeapType type = DEFAULT,
				D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON,
				D3D12_HEAP_FLAGS flag = D3D12_HEAP_FLAG_NONE) {
				return CreateGPUResource(desc, &CD3DX12_CLEAR_VALUE(desc.Format, cleardepth, clearstencil), type, state, flag);
			}
			GPUResource CreateRenderTargetGPUResource(
				DXGI_FORMAT format,
				uint32_t width, uint32_t height,
				uint32_t mipmaps = 0,
				uint32_t arraySize = 1,
				const float4& clearColor = {0.0f,0.0f,0.0f,0.0f});

			GPUResource CreateDepthStencilGPUResource(
				DXGI_FORMAT format,
				uint32_t width, uint32_t height,
				uint32_t mipmaps = 0,
				uint32_t arraySize = 1,
				float cleardepth = 1.0f, uint8_t clearstencil = 0);

			GPUResource CreateDefaultGPUResource(
				DXGI_FORMAT format,
				uint32_t width, uint32_t height,
				uint32_t mipmaps = 0,
				uint32_t arraySize = 1);

			GPUResource CreateUploadGPUResource(uint64_t bufferSize);


			Texture      CreateTexture(const GPUResource& gpuresource);
			ColorTexture CreateColorTexture(const GPUResource& gpuresource, float4 clearColor = { 0.0f,0.0f,0.0f,0.0f });
			DepthTexture CreateDepthTexture(const GPUResource& gpuresource, float cleardepth = 1.0f, uint8_t clearstencil = 0);
			void         CreateShader(const std::wstring& filename, Shader* shader, const std::vector<D3D_SHADER_MACRO>& macro = { {nullptr, nullptr} });

		public:
			bool CreateMeshDataFromFile(const std::string& filename, Mesh* mesh);
			bool CreateTextureFromFile(const std::string& filename, Texture* texture, bool is_cubmap = false, bool is_generate = false);
		public:
			static uint64_t GetEngineFrame() {
				return ms_EngineFrame;
			}

			static DXGI_FORMAT GetBackBufferFormat(){
				return ms_BackBufferFormat;
			}
			uint32_t GetDescriptorIncrementSize(EDescritporHeapType type) const;
			ID3D12Device* GetD3DDevice() {
				return m_Device.Get();
			}
		private:
			void UpdateRenderTarget();
			//
		};



	}









}