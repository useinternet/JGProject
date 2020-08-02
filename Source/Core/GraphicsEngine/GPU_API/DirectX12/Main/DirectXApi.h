#pragma once
#include "CommonCore.h"
#ifdef DX12_EXPORT

#define DX12_API __declspec(dllexport)

#else

#define DX12_API __declspec(dllimport)

#endif
// MACRO
#define DEPTH_BIAS_D32_FLOAT(d) (d/(1/pow(2,23)))
#define DX12_MAX_NUMDESCRIPTOR 1000000
namespace DX12
{
	enum
	{
		MAX_NUM_RENDERTARGET = 8,
	};
}

// defined
namespace DX12
{
	class DXDevice;
	class DXCommand;
	class Fence;
	class Resource;
	class Texture;
	class Buffer;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;
	class StructuredBuffer;

	class Shader;
	class VertexShader;
	class HullShader;
	class DomainShader;
	class PixelShader;
	class ComputeShader;

	class DescriptorAllocator;
	class DescriptorAllocation;
	class DynamicDescriptorAllocator;
	class _DXCommandResource;
	class CommandKey;
	class GraphicsCommandKey;
	class ComputeCommandKey;
	class CopyCommandKey;


	class DescriptorRange;
	class RootSignature;
	class PipelineState;
	class GraphicsPipelineState;
	class ComputePipelineState;
	class RenderTarget;
	class Mesh;
}

namespace DX12
{
	using CommandKeyPtr          = std::shared_ptr<CommandKey>;
	using GraphicsCommandKeyPtr = std::shared_ptr<GraphicsCommandKey>;
	using ComputeCommandKeyPtr  = std::shared_ptr<ComputeCommandKey>;
	using CopyCommandKeyPtr     = std::shared_ptr<CopyCommandKey>;

	

	using int2  = DirectX::XMINT2;
	using int3  = DirectX::XMINT3;
	using int4  = DirectX::XMINT4;
	using uint  = unsigned int;
	using uint2 = DirectX::XMUINT2;
	using uint3 = DirectX::XMUINT3;
	using uint4 = DirectX::XMUINT4;
	using float2 = DirectX::XMFLOAT2;
	using float3 = DirectX::XMFLOAT3;
	using float4 = DirectX::XMFLOAT4;
	using float3x3 = DirectX::XMFLOAT3X3;
	using float4x4 = DirectX::XMFLOAT4X4;
}

// define Datas

namespace DX12
{
	enum ShaderType
	{
		ShaderType_Vertex,
		ShaderType_Hull,
		ShaderType_Domain,
		ShaderType_Geometry,
		ShaderType_Pixel,
		ShaderType_Compute
	};

	enum ShaderDataType
	{
		_float_,
		_float2_,
		_float3_,
		_float4_,
		_int_,
		_int2_,
		_int3_,
		_int4_,
		_uint_,
		_uint2_,
		_uint3_,
		_uint4_,
		_float4x4_,
		_float3x3_,
		__struct__,
	};

	enum ShaderResourceType
	{
		__Texture2D__,
		__RWTexture2D__,
		__TextureCube__,
		__StructuredBuffer__,
		__RWStructuredBuffer__,
		__SamplerState__,
		__CBuffer__,
	};
	



	inline uint32_t GetShaderDataTypeByteSize(ShaderDataType type)
	{
		switch (type)
		{
		case _float_:
		case _int_:
		case _uint_:
			return 4;
		case _float2_:
		case _int2_:
		case _uint2_:
			return 8;
		case _float3_:
		case _int3_:
		case _uint3_:
			return 12;
		case _float4_:
		case _int4_:
		case _uint4_:
			return 16;
		case _float4x4_:
			return 64;
		case _float3x3_:
			return 36;
		default:
			return 0;
		}
	}

	inline std::string ToString(ShaderDataType type)
	{
		switch (type)
		{
		case _float_: return  "float";
		case _float2_: return "float2";
		case _float3_: return "float3";
		case _float4_: return "float4";
		case _int_: return  "int";
		case _int2_: return "int2";
		case _int3_: return "int3";
		case _int4_: return "int4";
		case _uint_: return  "uint";
		case _uint2_: return "uint2";
		case _uint3_: return "uint3";
		case _uint4_: return "uint4";
		case _float3x3_: return "float3x3";
		case _float4x4_: return "float4x4";
		default: return "none";
		}
	}

	struct ScreenInfo
	{
		HWND hWnd;
		int  width  = 0;
		int  height = 0;
	};

	class DX12_API ScissorRect
	{
		D3D12_RECT m_Rect;
	public:
		ScissorRect() = default;
		ScissorRect(int width, int height) {
			Set(width, height);
		}

		void Set(int width, int height) {
			m_Rect = CD3DX12_RECT(0, 0, width, height);
		}
		void Set(int left, int top, int right, int bottom) {
			m_Rect = CD3DX12_RECT(left, top, right, bottom);
		}
		const D3D12_RECT& Get() const {
			return m_Rect;
		}
	};

	class DX12_API Viewport
	{
	private:
		D3D12_VIEWPORT m_Viewport;

	public:
		Viewport() = default;
		Viewport(float width, float height) {
			Set(width, height);
		}
		void Set(float width, float height) {
			m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, width, height);
		}
		void Set(float topLeftX, float topLeftY, float width, float height) {
			m_Viewport = CD3DX12_VIEWPORT(topLeftX, topLeftY, width, height);
		}
		const D3D12_VIEWPORT& Get() const {
			return m_Viewport;
		}
	};
}
// Test
namespace DX12
{
	class  DX12_API DXDevice
	{
	public:
		static void Create(int bufferCount = 3);
		static void BeginFrame();
		static void EndFrame();
		static void Destroy();

		// Ä¿¸Çµå Å° ¿äÃ»
		static GraphicsCommandKeyPtr RequestGraphicsCommandKey(int priority = 0);
		static ComputeCommandKeyPtr  RequestComputeCommandKey(int priority = 0);
		static CopyCommandKeyPtr     RequestCopyCommandKey(int priority = 0);


		// Screen »ý¼º ¿äÃ»
		static void       RegisterScreen(HWND hWnd, int width, int height, const DirectX::XMFLOAT4& clearColor = { 0.0F,0.0F,0.0F,0.0F });
		static void       UnRegisterScreen(HWND hWnd);
		static void       RequestSubmitTextureToScreen(HWND hWnd, Texture& texture);
		static void       RequestResizeScreen(HWND hWnd, int width, int height);
		static ScreenInfo GetScreenInfo(HWND hWnd);


		// Shader »ý¼º
		static VertexShader  CreateVertexShaderFromFile(const std::string& path, const std::vector<std::pair<std::string, std::string>>& definedMacros);
		static HullShader    CreateHullShaderFromFile(const std::string& path, const std::vector<std::pair<std::string, std::string>>& definedMacros);
		static DomainShader  CreateDomainShaderFromFile(const std::string& path, const std::vector<std::pair<std::string, std::string>>& definedMacros);
		static PixelShader   CreatePixelShaderFromFile(const std::string& path, const std::vector<std::pair<std::string, std::string>>& definedMacros);
		static ComputeShader CreateComputeShaderFromFile(const std::string& path, const std::vector<std::pair<std::string, std::string>>& definedMacros);


		

		// D3D ComPtr 
		static ID3D12Device* GetD3DDevice();
		static IDXGIFactory4* GetDXGIFactory();


		static Texture LoadRegisteredTexture(const std::wstring& name);

		// 
		static uint64_t GetBufferIndex();
		static uint64_t GetBufferCount();
	};
}
// DXCommand
namespace DX12
{
	class  DX12_API DXCommand
	{
	public:
		// Grahics
		static void SetViewport(GraphicsCommandKeyPtr cmdKey, const Viewport& viewport);
		static void SetViewports(GraphicsCommandKeyPtr cmdKey, const std::vector<Viewport>& viewports);
		static void SetScissorRect(GraphicsCommandKeyPtr cmdKey, const ScissorRect& rect);
		static void SetScissorRects(GraphicsCommandKeyPtr cmdKey, const std::vector<ScissorRect>& rects);
		static void ClearRenderTarget(GraphicsCommandKeyPtr cmdKey, RenderTarget& renderTarget, bool is_depthClear = true);
		static void SetRenderTarget(GraphicsCommandKeyPtr cmdKey, RenderTarget& renderTarget);
		static void BindRootSignature(GraphicsCommandKeyPtr cmdKey, RootSignature& rootSig);
		static void BindPipelineState(GraphicsCommandKeyPtr cmdKey, GraphicsPipelineState& pso);
		//
		static void BindTexture(GraphicsCommandKeyPtr cmdKey, uint32_t rootParam, Texture& texture, void* desc = nullptr);
		static void BindTextures(GraphicsCommandKeyPtr cmdKey, uint32_t rootParam, Texture* textures, void** desc, uint32_t textureCount);
		static void BindConstantBuffer(GraphicsCommandKeyPtr cmdKey,uint32_t rootparam, ConstantBuffer& buffer);
		static void BindDynamicConstantBuffer(GraphicsCommandKeyPtr cmdKey, uint32_t rootparam, const void* data, size_t  elementSize);


		static void BindStructuredBuffer(GraphicsCommandKeyPtr cmdKey, uint32_t rootparam, StructuredBuffer& buffer);
		static void BindDynamicStructuredBuffer(GraphicsCommandKeyPtr cmdKey, uint32_t rootparam, const  void* data, size_t elementSize, size_t elementCount);

		static void BindConstants(GraphicsCommandKeyPtr cmdKey, uint32_t rootparam, uint32_t btSize,  void* data, uint32_t offset = 0);

		static void BindVertexBuffer(GraphicsCommandKeyPtr cmdKey, VertexBuffer& vBuffer);
		static void BindDynamicVertexBuffer(GraphicsCommandKeyPtr cmdKey, void* data, size_t elementSize, size_t elementCount);

	

		static void BindIndexBuffer(GraphicsCommandKeyPtr cmdKey, IndexBuffer& iBuffer);
		static void BindDynamicIndexBuffer(GraphicsCommandKeyPtr cmdKey, const std::vector<uint32_t>& indices);
		static void BindDynamicIndexBuffer(GraphicsCommandKeyPtr cmdKey, const uint32_t* iData, size_t count);
		static void SetPrimitiveTopology(GraphicsCommandKeyPtr cmdKey, D3D_PRIMITIVE_TOPOLOGY topology);
		static void Draw(GraphicsCommandKeyPtr cmdKey,
			uint32_t vertexCount, uint32_t instanceCount, 
			uint32_t startVertexLocation = 0, uint32_t startInstanceLocation = 0);
		static void DrawIndexed(GraphicsCommandKeyPtr cmdKey,
			uint32_t indexCount, uint32_t  instanceCount = 1, 
			uint32_t startIndexLocation = 0, uint32_t startVertexLocation = 0, uint32_t startInstanceLocation =0);


		// Compute

		static void BindRootSignature(ComputeCommandKeyPtr cmdKey, RootSignature& rootSig);
		static void BindPipelineState(ComputeCommandKeyPtr cmdKey, ComputePipelineState& pso);
	
		static void BindTexture(ComputeCommandKeyPtr cmdKey, uint32_t rootParam, Texture& texture, void* desc = nullptr);
		static void BindTextures(ComputeCommandKeyPtr cmdKey, uint32_t rootParam, Texture* textures, void** desc, uint32_t textureCount);
		static void BindConstantBuffer(ComputeCommandKeyPtr cmdKey, uint32_t rootparam, ConstantBuffer& buffer);
		static void BindDynamicConstantBuffer(ComputeCommandKeyPtr cmdKey, uint32_t rootparam, const  void* data, size_t  elementSize);


		static void BindStructuredBuffer(ComputeCommandKeyPtr cmdKey, uint32_t rootparam, StructuredBuffer& buffer);
		static void BindDynamicStructuredBuffer(ComputeCommandKeyPtr cmdKey, uint32_t rootparam, const  void* data, size_t elementSize, size_t elementCount);

		static void BindConstants(ComputeCommandKeyPtr cmdKey, uint32_t rootparam, uint32_t btSize, void* data, uint32_t offset = 0);
		static void Dispatch(ComputeCommandKeyPtr cmdKey, uint32_t xgroupCount, uint32_t ygroupCount, uint32_t zgroupCount);
		
		// Common

		static Texture LoadTextureFromFile(CommandKeyPtr cmdKey, const std::wstring& path, bool is_cube);


		static void CopyResource(CommandKeyPtr cmdKey, Resource& dest, Resource& src);
		
		static void CopyBuffer(CommandKeyPtr cmdKey, Buffer& buffer, void* data, size_t elementSize, size_t elementCount);

		static void CopyConstantBuffer(CommandKeyPtr cmdKey, ConstantBuffer& buffer, void* data, size_t elementSize);
		static void CopyStructuredBuffer(CommandKeyPtr cmdKey, StructuredBuffer& buffer, void* data, size_t elementSize, size_t elementCount);
		static void CopyVertexBuffer(
			CommandKeyPtr cmdKey, VertexBuffer& vBuffer,
			void* data, size_t elementSize, size_t elementCount);

		static void CopyIndexBuffer(
			CommandKeyPtr cmdKey, IndexBuffer& iBuffer,
			void* data, size_t elementCount);





		// Resource State
		static void FlushResourceBarrier(CommandKeyPtr cmdKey);
		static void TransitionBarrier(
			CommandKeyPtr cmdKey,
			const Resource& rsc, D3D12_RESOURCE_STATES state,
			uint32_t subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flush = true);
		static void UAVBarrier(CommandKeyPtr cmdKey, const Resource& rsc, bool flush = true);
		static void AliasBarrier(CommandKeyPtr cmdKey, const Resource& before_rsc, const Resource& after_rsc, bool flush = true);
	};

	class  CommandKey
	{
		_DXCommandResource* cmdResource = nullptr;
	private:
		friend 	void BindTexture(CommandKeyPtr cmdKey, uint32_t rootParam, Texture& texture, void* desc);
		friend  void BindTextures(CommandKeyPtr cmdKey, uint32_t rootParam, Texture* texture,
			void** desc, uint32_t textureCount);
	private:
		friend DXDevice;
		friend DXCommand;
		friend class Screen;
	};
	class  GraphicsCommandKey : public CommandKey
	{
		friend DXDevice;
		friend DXCommand;
		friend class Screen;
	};
	class  ComputeCommandKey : public CommandKey
	{
		friend DXDevice;
		friend DXCommand;
		friend class Screen;
	};
	class  CopyCommandKey : public CommandKey
	{
		friend DXDevice;
		friend DXCommand;
		friend class Screen;
	};
}
// DX12 
namespace DX12
{



	class DescriptorAllocator
	{
	public:
		DescriptorAllocation Allocate();
		void                 UpdatePage();
	public:
		class CPUDescriptorPage
		{
		public:
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> D3DDescriptorHeap;
			D3D12_DESCRIPTOR_HEAP_TYPE                   D3DType;
			std::queue<D3D12_CPU_DESCRIPTOR_HANDLE>      AbandonedHandles;
			std::map<uint64_t, std::queue<D3D12_CPU_DESCRIPTOR_HANDLE>> PendingHandles;
			uint64_t HeapSize = 0;
			uint64_t Offset = 0;
			std::mutex FreeMutex;
		public:
			bool HasSpace();
			DescriptorAllocation Allocate();
			void UpdatePage();
			void Free(D3D12_CPU_DESCRIPTOR_HANDLE handle);

		};
	public:
		DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint64_t sizeperHeap = 1024) :
			m_D3DType(type), m_SizePerHep(sizeperHeap)
		{

		}
	private:
		CPUDescriptorPage* RequestPage();
	private:
		uint64_t                       m_SizePerHep = 1024;
		D3D12_DESCRIPTOR_HEAP_TYPE     m_D3DType;
		std::vector<std::unique_ptr<CPUDescriptorPage>> m_Pages;
		std::mutex                     m_AllocateMutex;

	};

	class DescriptorAllocation
	{
		friend DescriptorAllocator::CPUDescriptorPage;
	private:
		DescriptorAllocation(const DescriptorAllocation& copy) = delete;
		DescriptorAllocation& operator=(const DescriptorAllocation& rhs) = delete;
	public:
		DescriptorAllocation() {}
		~DescriptorAllocation();
		DescriptorAllocation(DescriptorAllocation&& rhs);
		DescriptorAllocation& operator=(DescriptorAllocation&& rhs); 
	public:
		D3D12_CPU_DESCRIPTOR_HANDLE CPU() const {
			return _CPU;
		}
		bool IsValid() const {
			return _CPU.ptr != 0;
		}
	private:
		D3D12_CPU_DESCRIPTOR_HANDLE _CPU;
		DescriptorAllocator::CPUDescriptorPage* _OwnerPage = nullptr;
	};


	class UploadAllocator
	{
	public:
		// CPU ï¿½Ö¼Ò¿ï¿½ GPU ï¿½Ö¼Ò°ï¿½ ï¿½ï¿½ï¿½ï¿½Ö´ï¿?ï¿½ï¿½ï¿½ï¿½Ã¼ 
		struct Allocation
		{
			void* CPU;
			D3D12_GPU_VIRTUAL_ADDRESS GPU;
		};
	private:
		class Page
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> m_D3D12Resource;
			//
			void* m_CPUptr;
			D3D12_GPU_VIRTUAL_ADDRESS m_GPUptr;
			//
			size_t m_PageSize;
			size_t m_Offset;
		public:
			Page(ID3D12Device* device, size_t sizeInBytes);
			~Page();

			/*
			ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Ã»ï¿½ï¿½ ï¿½Þ¸ï¿½ ï¿½Ò´ï¿½ï¿½Ò¼ï¿½ ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ö´ï¿½ï¿½ï¿½ Ã¼Å© ï¿½ï¿½ï¿½Ø´ï¿½. */
			bool HasSpace(size_t sizeInBytes, size_t alignment) const;

			/*
			ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ò´ï¿½ï¿½Ñ´ï¿½.*/
			Allocation Allocate(size_t sizeInBytes, size_t alignment);

			// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
			void Reset();

		};
		using PagePool = std::deque<std::shared_ptr<Page>>;

		PagePool m_PagePool;
		PagePool m_AvailablePages;

		std::shared_ptr<Page> m_CurrentPage;

		size_t m_PageSize;

	public:
		explicit UploadAllocator(size_t pageSize = 2 * 1024 * 1024);
		virtual ~UploadAllocator();
	public:
		size_t GetPageSize() const { return m_PageSize; }
		Allocation Allocate(size_t sizeInBytes, size_t alignment);
		void Reset();
	private:
		std::shared_ptr<Page> RequestPage();
	};

	/*
	1. GPU Àç»ç¿ë
	- DrawÇÏ°í ÇÏ³ª¶óµµ ´Ù¸¥ rootParam CommitÇÒ °æ¿ì GPU  ÁÖ¼Ò ²¿ÀÓ
	2. DescriptorHeap resize
	
	
	*/
	class DynamicDescriptorAllocator
	{
	public:
		DynamicDescriptorAllocator(uint32_t numDescriptor = 1024);
	public:
		void CommitRootSignature(RootSignature& rootSig);
		void CommitDescriptorTable(uint32_t rootParam, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles);

		void Reset();
		void PushDescriptorTable(_DXCommandResource* cmdResource, bool is_graphics);
		int  GetDescriptorInitAsType(uint32_t rootParam) const {
			if (m_RootParamInitTypeMap.find(rootParam) == m_RootParamInitTypeMap.end())
				assert("DynamicDescriptorAllocator::GetDescriptorInitAsType  :  NonExistent rootParam");
			return m_RootParamInitTypeMap.at(rootParam);
		}
		D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorTableType(uint32_t rootParam) const {
			if (m_DescriptorTableType.find(rootParam) == m_DescriptorTableType.end())
				assert("DynamicDescriptorAllocator::GetDescriptorTableType  :  NonExistent rootParam");
			return m_DescriptorTableType.at(rootParam);
		}
	private:
		void RequestDescriptorHeap();
	private:
		struct CPUCache
		{
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> cpuHandles;
			uint32_t numDescriptor = 0;
		};
		struct D3DHeap
		{
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorHeap;
			uint32_t numDescriptor = 1024;
		};
	private:
		D3DHeap            m_CurrentD3DHeap;
		std::map<int, int> m_RootParamInitTypeMap;
		std::map<uint32_t, D3D12_DESCRIPTOR_RANGE_TYPE> m_DescriptorTableType;
		std::map<int, CPUCache>           m_CPUCache;
		uint32_t m_PushedHandleOffset = 0;
		bool m_ResizeDirty = false;
		//  uint32_t           m_CommittedCPUHandleCount = 0;
	    //	uint32_t           m_CommittedNumDescriptor = 0;
		uint32_t m_IncreaseSize = 0;
	};
}
// Shader
namespace DX12
{
	class  DX12_API Shader
	{
	public:
		const std::string& GetName() const {
			return m_Name;
		}

		const std::string& GetSourceCode() const {
			return m_SourceCode;
		}
		const std::string GetErrorCode() const {
			return m_ErrorCode;
		}
		void* GetPtr() const {
			if (m_Data) return m_Data->GetBufferPointer();
			else        return nullptr;
		}
		uint64_t GetSize() const {
			if (m_Data) return m_Data->GetBufferSize();
			else return 0;
		}
		void SetName(const std::string& name) {
			m_Name = name;
		}
		void SetSourceCode(const std::string& source_code) {
			m_SourceCode = source_code;
			m_IsDirty = true;
		}
		void AddMacro(const std::string& define, const std::string& value) {
			m_DefinedMacros.push_back(std::pair<std::string, std::string>(define, value));
			m_IsDirty = true;
		}
		bool IsValid() const {
			return m_Data != nullptr;
		}
		bool Compile(const std::string& entry, const std::string& version = "5_0", ID3DInclude* include = D3D_COMPILE_STANDARD_FILE_INCLUDE);
	public:
		virtual ShaderType GetType() const {
			return (ShaderType)-1;
		}
	private:
		std::string  m_Name = "Shader";
		std::string  m_SourceCode;
		std::string  m_ErrorCode;
		std::vector<std::pair<std::string, std::string>> m_DefinedMacros;
		Microsoft::WRL::ComPtr<ID3DBlob> m_Data;

		bool m_IsDirty = false;
	};




	class  DX12_API VertexShader : public Shader
	{
	public:
		virtual ShaderType GetType() const {
			return ShaderType_Vertex;
		}
	};
	class  DX12_API HullShader : public Shader
	{
	public:
		virtual ShaderType GetType() const {
			return ShaderType_Hull;
		}
	};
	class  DX12_API DomainShader : public Shader
	{
	public:
		virtual ShaderType GetType() const {
			return ShaderType_Domain;
		}
	};
	class  DX12_API GeometryShader : public Shader
	{
	public:
		virtual ShaderType GetType() const {
			return ShaderType_Geometry;
		}
	};
	class  DX12_API PixelShader : public Shader
	{
	public:
		virtual ShaderType GetType() const {
			return ShaderType_Pixel;
		}
	};
	class  DX12_API ComputeShader : public Shader
	{
	public:
		virtual ShaderType GetType() const {
			return ShaderType_Compute;
		}
	};



}


// Resource
namespace DX12
{
	class DX12_API  Resource
	{
		friend void RegisterResource(Resource& src, const std::wstring& name, ID3D12Resource* d3d_resource, D3D12_RESOURCE_STATES initState);
		friend void DecreaseResourceRefCount(ID3D12Resource* d3d_resource);
	public:
		Resource() = default;
		Resource(const Resource& copy);
		Resource(Resource&& rhs);
		virtual ~Resource();

		Resource& operator=(const Resource& rhs);
		Resource& operator=(Resource&& rhs);
	public:
		virtual void Create(const std::wstring& name, const D3D12_RESOURCE_DESC& desc,
			D3D12_RESOURCE_STATES  initState = D3D12_RESOURCE_STATE_COMMON,
			D3D12_CLEAR_VALUE* clearValue = nullptr,
			D3D12_HEAP_PROPERTIES  heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAGS       heapFlags = D3D12_HEAP_FLAG_NONE);


		const std::wstring& GetName() const {
			return m_Name;
		}
		void SetName(const std::wstring& name) {
			if (m_D3DResource == nullptr) return;
			m_Name = name;
			m_D3DResource->SetName(m_Name.c_str());
		}


		D3D12_RESOURCE_DESC GetDesc() const {
			return m_D3DResource->GetDesc();
		}

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
			if (m_D3DResource != nullptr) return m_D3DResource->GetGPUVirtualAddress();
			else return 0;

		}

		bool IsValid() const {
			return m_D3DResource != nullptr;
		}

		ID3D12Resource* GetD3DPtr() const {
			return m_D3DResource.Get();
		}
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_D3DResource;
		std::wstring m_Name;
	private:
		friend void DXDevice::Destroy();
	};
	class  DX12_API Texture : public Resource
	{
	public:
		Texture() = default;
		Texture(const Texture& copy);
		Texture(Texture&& rhs);
		virtual ~Texture() = default;
		Texture& operator=(const Texture& rhs);
		Texture& operator=(Texture&& rhs);
	public:
		virtual void Create(const std::wstring& name, const D3D12_RESOURCE_DESC& desc,
			D3D12_RESOURCE_STATES  initState = D3D12_RESOURCE_STATE_COMMON,
			D3D12_CLEAR_VALUE* clearValue = nullptr,
			D3D12_HEAP_PROPERTIES  heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAGS       heapFlags = D3D12_HEAP_FLAG_NONE) override;

	
		void CreateTexture(const std::wstring& name, DXGI_FORMAT format,
			uint32_t width, uint32_t height, uint32_t miplevel = 0, uint32_t arraySize = 1);


		void CreateUnordoredAccessTexture(const std::wstring& name, DXGI_FORMAT format,
			uint32_t width, uint32_t height, uint32_t miplevel = 0, uint32_t arraySize = 1);

		void CreateRenderTargetTexture(const std::wstring& name, DXGI_FORMAT format,
			uint32_t width, uint32_t height, uint32_t miplevel = 0, uint32_t arraySize = 1,
			const float4& clearColor = { 0.0f,0.0f,0.0f,0.0f });


		void CreateDepthStencilTexture(const std::wstring& name, DXGI_FORMAT format,
			uint32_t width, uint32_t height, uint32_t miplevel = 0, uint32_t arraySize = 1,
			float clear_depth = 1.0f, uint8_t clear_stencil = 0);
	public:
		D3D12_CPU_DESCRIPTOR_HANDLE GetRTV(D3D12_RENDER_TARGET_VIEW_DESC* desc = nullptr)    const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetSRV(D3D12_SHADER_RESOURCE_VIEW_DESC* desc = nullptr)  const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDSV(D3D12_DEPTH_STENCIL_VIEW_DESC* desc = nullptr)    const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetUAV(D3D12_UNORDERED_ACCESS_VIEW_DESC* desc = nullptr) const;

		void Resize(uint32_t width, uint32_t height);


		D3D12_CLEAR_VALUE* GetClearValue() const {
			return m_ClearValue.get();
		}
	private:
		void CreateView();
	private:
		mutable std::unordered_map<size_t, DescriptorAllocation> m_RTVs;
		mutable std::unordered_map<size_t, DescriptorAllocation> m_SRVs;
		mutable std::unordered_map<size_t, DescriptorAllocation> m_DSVs;
		mutable std::unordered_map<size_t, DescriptorAllocation> m_UAVs;


		mutable std::mutex m_RTVMutex;
		mutable std::mutex m_SRVMutex;
		mutable std::mutex m_DSVMutex;
		mutable std::mutex m_UAVMutex;


		std::unique_ptr<D3D12_CLEAR_VALUE> m_ClearValue;
	};



	class DX12_API  Buffer : public Resource
	{

	public:
		virtual void Create(
			const std::wstring& name, uint32_t btSize,
			D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);


	};
	class  DX12_API UploadBuffer : public Buffer
	{
	public:
		virtual void Create(
			const std::wstring& name, uint32_t btSize,
			D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) override;
	};




	class  DX12_API VertexBuffer : public Buffer
	{
	public:
		const D3D12_VERTEX_BUFFER_VIEW& Get() const {
			return m_View;
		}
	private:
		friend void  DXCommand::CopyVertexBuffer(
			CommandKeyPtr cmdKey, VertexBuffer& vBuffer,
			void* data, size_t elementSize, size_t elementCount);


	private:
		D3D12_VERTEX_BUFFER_VIEW m_View;
		uint32_t m_ElementSize  = 0;
		uint32_t m_ElmenetCount = 0;
	};
	class  DX12_API IndexBuffer : public Buffer
	{
	public:
		const D3D12_INDEX_BUFFER_VIEW& Get() const {
			return m_View;
		}
	private:
		friend void  DXCommand::CopyIndexBuffer(
			CommandKeyPtr cmdKey, IndexBuffer& iBuffer,
			void* data, size_t elementCount);
	private:
		D3D12_INDEX_BUFFER_VIEW m_View;
		uint32_t m_ElementSize  = 0;
		uint32_t m_ElementCount = 0;
	};



	class  DX12_API ConstantBuffer : public UploadBuffer
	{
	public:
		virtual void Create(
			const std::wstring& name, uint32_t btSize,
			D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) override;
	public:
		D3D12_CPU_DESCRIPTOR_HANDLE GetCBV() const; 
	private:
		DescriptorAllocation m_CBV;
	private:
		friend void DXCommand::CopyConstantBuffer(CommandKeyPtr cmdKey, ConstantBuffer& buffer, void* data, size_t elementSize);
	};

	class  DX12_API StructuredBuffer : public Buffer
	{

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetUAV() const;
	private:
		DescriptorAllocation m_SRV;
		DescriptorAllocation m_UAV;
		uint32_t m_ElementCount = 0;
		uint32_t m_ElementSize  = 0;
	private:
		friend void DXCommand::CopyStructuredBuffer(CommandKeyPtr cmdKey, StructuredBuffer& buffer, void* data, size_t elementSize, size_t elementCount);
	};
}


namespace DX12
{
	class  DX12_API RootSignature
	{
		friend DynamicDescriptorAllocator;
	public:
		enum
		{
			__DescriptorTable__,
			__ShaderResourceView__,
			__UnorderedAccessView__,
			__ConstantBufferView__,
			__Constant__
		};
		struct DescriptorTableInfo
		{
			D3D12_DESCRIPTOR_RANGE_TYPE type;
			uint32_t numDescirptor;
		};
	public:  // 0, 1, 2, 3, 4, 5,
		// 000
		void InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE type, uint32_t numDescriptor, uint32_t register_num,
			uint32_t register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			uint32_t rootParam = (uint32_t)m_RootSigInitType.size();
			DescriptorTableInfo info;
			info.numDescirptor = numDescriptor;
			info.type = type;
			m_DescriptorTableInfoByRootParam[rootParam] = info;

			auto range = std::make_shared<CD3DX12_DESCRIPTOR_RANGE>();
			range->Init(type, numDescriptor, register_num, register_space);

			CD3DX12_ROOT_PARAMETER param;
			param.InitAsDescriptorTable(1, range.get(), visibility);
			m_RootParams.push_back(param);
			m_DescriptorRanges.push_back(range);

			m_RootSigInitType.push_back(__DescriptorTable__);
		}
		void InitAsSRV(uint32_t register_num, uint32_t register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			CD3DX12_ROOT_PARAMETER param;

			param.InitAsShaderResourceView(register_num, register_space, visibility);
			m_RootParams.push_back(param);


		
			m_RootSigInitType.push_back(__ShaderResourceView__);
		}
		void InitAsUAV(uint32_t register_num, uint32_t register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			CD3DX12_ROOT_PARAMETER param;

			param.InitAsUnorderedAccessView(register_num, register_space, visibility);
			m_RootParams.push_back(param);

			m_RootSigInitType.push_back(__UnorderedAccessView__);
		}
		void InitAsCBV(uint32_t register_num, uint32_t register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			CD3DX12_ROOT_PARAMETER param;

			param.InitAsConstantBufferView(register_num, register_space, visibility);
			m_RootParams.push_back(param);

			m_RootSigInitType.push_back(__ConstantBufferView__);
		}
		void InitAsConstant(uint32_t btSize, uint32_t register_num, uint32_t register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			CD3DX12_ROOT_PARAMETER param;

			param.InitAsConstants(btSize / 4, register_num, register_space, visibility);
			m_RootParams.push_back(param);

			m_RootSigInitType.push_back(__Constant__);
		}
		void AddStaticSamplerState(const CD3DX12_STATIC_SAMPLER_DESC& desc)
		{
			m_SamplerState.push_back(desc);
		}
		DescriptorTableInfo GetDescriptorTableRangeType(uint32_t rootparam) const {
			if (m_DescriptorTableInfoByRootParam.find(rootparam) == m_DescriptorTableInfoByRootParam.end())
				assert("GetDescriptorTableInfo : nonexist DescriptorTable in rootparam");
			else
			{
				return m_DescriptorTableInfoByRootParam.at(rootparam);
			}
			return DescriptorTableInfo();
		}
		bool Finalize();
	public:
		ID3D12RootSignature* GetD3DPtr() const {
			return m_D3D_RootSig.Get();
		}
	private:


		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_D3D_RootSig;
		std::vector<CD3DX12_ROOT_PARAMETER>         m_RootParams;
		std::vector<D3D12_STATIC_SAMPLER_DESC>      m_SamplerState;
		std::vector<int> m_RootSigInitType;
		std::map<uint32_t, DescriptorTableInfo> m_DescriptorTableInfoByRootParam;
		std::vector<std::shared_ptr<D3D12_DESCRIPTOR_RANGE>> m_DescriptorRanges;
	};

	class  DX12_API InputLayout
	{
	private:
		friend GraphicsPipelineState;
	public:
		void Add(const char* sementicName, ShaderDataType type, uint32_t sementic_slot)
		{
			D3D12_INPUT_ELEMENT_DESC desc;
			desc.SemanticName = sementicName;
			desc.SemanticIndex = sementic_slot;
			desc.InputSlot = 0;
			desc.AlignedByteOffset = m_BtOffset;
			desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;

			DXGI_FORMAT format;
			switch (type)
			{
			case _float_:
				format = DXGI_FORMAT_R32_FLOAT;    break;
			case _float2_:
				format = DXGI_FORMAT_R32G32_FLOAT; break;
			case _float3_:
				format = DXGI_FORMAT_R32G32B32_FLOAT;    break;
			case _float4_:
				format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;

			case _uint_:
				format = DXGI_FORMAT_R32_UINT;    break;
			case _uint2_:
				format = DXGI_FORMAT_R32G32_UINT; break;
			case _uint3_:
				format = DXGI_FORMAT_R32G32B32_UINT;    break;
			case _uint4_:
				format = DXGI_FORMAT_R32G32B32A32_UINT; break;

			case _int_:
				format = DXGI_FORMAT_R32_SINT; break;
			case _int2_:
				format = DXGI_FORMAT_R32G32_SINT; break;
			case _int3_:
				format = DXGI_FORMAT_R32G32B32_SINT; break;
			case _int4_:
				format = DXGI_FORMAT_R32G32B32A32_SINT; break;
			default:
				format = DXGI_FORMAT_UNKNOWN;
			}
			desc.Format = format;
			m_BtOffset += GetShaderDataTypeByteSize(type);
			m_ElementDesc.push_back(desc);
		}
	private:
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_ElementDesc;
		uint32_t m_BtOffset = 0;
	};

	class  PipelineState
	{
	public:
		ID3D12PipelineState* GetD3DPtr() const {
			return m_D3D_PSO.Get();
		}
		virtual bool Finalize() = 0;
	protected:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_D3D_PSO;
	};



	class  DX12_API GraphicsPipelineState : public PipelineState
	{
	public:
		GraphicsPipelineState();
	public:
		void BindRootSignature(const RootSignature& rootSig);
		void BindRenderTarget(RenderTarget& renderTarget);
		void BindRenderTarget(const std::vector<DXGI_FORMAT>& rtFormat, DXGI_FORMAT dvFormat = DXGI_FORMAT_UNKNOWN);
		void BindShader(ShaderType type, const Shader& shader);
		void BindInputLayout(const InputLayout& inputLayout);
		void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
		void SetSampleMask(uint32_t sampleMask);
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& desc);
		void SetBlendState(const D3D12_BLEND_DESC& desc);
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc);

		virtual bool Finalize() override;
	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_Desc = {};
		InputLayout m_InputLayout;
	
	};

	class  DX12_API ComputePipelineState : public PipelineState
	{
	public:
		void BindRootSignature(const RootSignature& rootSig);
		void BindShader(const Shader& shader);

		virtual bool Finalize() override;
	private:
		D3D12_COMPUTE_PIPELINE_STATE_DESC m_Desc = {};
	};

	class  DX12_API RenderTarget
	{
	private:
		using RTVDescPtr = std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC>;
		using DSVDescPtr = std::unique_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC>;
	public:
		RenderTarget() {}
		RenderTarget(const RenderTarget& rt)
		{
			for (int i = 0; i < MAX_NUM_RENDERTARGET; ++i)
			{
				m_Textures[i] = rt.m_Textures[i];
				if (m_RTVDescs[i]) m_RTVDescs[i].reset();
				if (rt.m_RTVDescs[i])
				{
					m_RTVDescs[i] = std::make_unique<D3D12_RENDER_TARGET_VIEW_DESC>(*rt.m_RTVDescs[i]);
				}

			}
			m_DepthTexture = rt.m_DepthTexture;
			if (rt.m_DSVDesc)
			{
				if (m_DSVDesc) m_DSVDesc.reset();
				m_DSVDesc = std::make_unique<D3D12_DEPTH_STENCIL_VIEW_DESC>(*rt.m_DSVDesc);
			}
		}
		RenderTarget& operator=(const RenderTarget& rt)
		{
			for (int i = 0; i < MAX_NUM_RENDERTARGET; ++i)
			{
				m_Textures[i] = rt.m_Textures[i];
				if (rt.m_RTVDescs[i])
				{
					m_RTVDescs[i] = std::make_unique<D3D12_RENDER_TARGET_VIEW_DESC>(*rt.m_RTVDescs[i]);
				}
			
			}
			m_DepthTexture = rt.m_DepthTexture;
			if (rt.m_DSVDesc)
			{
				m_DSVDesc = std::make_unique<D3D12_DEPTH_STENCIL_VIEW_DESC>(*rt.m_DSVDesc);
			}
			return *this;
		}
		Texture& operator[](int i)
		{
			return m_Textures[i];
		}
		void Resize(int width, int height)
		{
			for (int i = 0; i < MAX_NUM_RENDERTARGET; ++i)
			{
				if (m_Textures[i].IsValid())
				{
					m_Textures[i].Resize(width, height);
				}
			}
			if (m_DepthTexture.IsValid())
				m_DepthTexture.Resize(width, height);
		}
		void SetRTVDesc(int slot, const D3D12_RENDER_TARGET_VIEW_DESC& desc) {
			if(m_RTVDescs[slot] == nullptr)
				m_RTVDescs[slot] = std::make_unique<D3D12_RENDER_TARGET_VIEW_DESC>();
			 
			*m_RTVDescs[slot] = desc;
		}

		D3D12_RENDER_TARGET_VIEW_DESC* GetRTVDesc(int slot) const {
			return m_RTVDescs[slot].get();
		}

		void SetDepthStencilTexture(const Texture& texture)
		{
			m_DepthTexture = texture;
		}
		Texture& GetDepthStencilTexture()
		{
			return m_DepthTexture;
		}
		const Texture& GetDepthStencilTexture() const
		{
			return m_DepthTexture;
		}
		void SetDSVDesc(const D3D12_DEPTH_STENCIL_VIEW_DESC& desc) {
			if(m_DSVDesc == nullptr)
				m_DSVDesc = std::make_unique<D3D12_DEPTH_STENCIL_VIEW_DESC>();

			*m_DSVDesc = desc;

		}
		D3D12_DEPTH_STENCIL_VIEW_DESC* GetDSVDesc() const {
			return m_DSVDesc.get();
		}
	private:
		Texture    m_Textures[MAX_NUM_RENDERTARGET];
		RTVDescPtr m_RTVDescs[MAX_NUM_RENDERTARGET];
		Texture    m_DepthTexture;
		DSVDescPtr m_DSVDesc;
	};
}


namespace std
{
	template <typename T>
	inline void dx_12_hash_combine(std::size_t& seed, const T& v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}


	template<>
	struct hash<D3D12_RENDER_TARGET_VIEW_DESC>
	{
		size_t operator()(const D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc) const noexcept
		{
			size_t seed = 0;

			dx_12_hash_combine(seed, rtvDesc.Format);
			dx_12_hash_combine(seed, rtvDesc.ViewDimension);

			switch (rtvDesc.ViewDimension)
			{
			case D3D12_RTV_DIMENSION_BUFFER:
				dx_12_hash_combine(seed, rtvDesc.Buffer.FirstElement);
				dx_12_hash_combine(seed, rtvDesc.Buffer.NumElements);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE1D:
				dx_12_hash_combine(seed, rtvDesc.Texture1D.MipSlice);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE1DARRAY:
				dx_12_hash_combine(seed, rtvDesc.Texture1DArray.ArraySize);
				dx_12_hash_combine(seed, rtvDesc.Texture1DArray.FirstArraySlice);
				dx_12_hash_combine(seed, rtvDesc.Texture1DArray.MipSlice);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE2D:
				dx_12_hash_combine(seed, rtvDesc.Texture2D.MipSlice);
				dx_12_hash_combine(seed, rtvDesc.Texture2D.PlaneSlice);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
				dx_12_hash_combine(seed, rtvDesc.Texture2DArray.ArraySize);
				dx_12_hash_combine(seed, rtvDesc.Texture2DArray.FirstArraySlice);
				dx_12_hash_combine(seed, rtvDesc.Texture2DArray.MipSlice);
				dx_12_hash_combine(seed, rtvDesc.Texture2DArray.PlaneSlice);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE2DMS:
				dx_12_hash_combine(seed, rtvDesc.Texture2DMS.UnusedField_NothingToDefine);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
				dx_12_hash_combine(seed, rtvDesc.Texture2DMSArray.ArraySize);
				dx_12_hash_combine(seed, rtvDesc.Texture2DMSArray.FirstArraySlice);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE3D:
				dx_12_hash_combine(seed, rtvDesc.Texture3D.FirstWSlice);
				dx_12_hash_combine(seed, rtvDesc.Texture3D.MipSlice);
				dx_12_hash_combine(seed, rtvDesc.Texture3D.WSize);
				break;
			}

			return seed;
		}
	};


	template<>
	struct hash<D3D12_DEPTH_STENCIL_VIEW_DESC>
	{
		std::size_t operator()(const D3D12_DEPTH_STENCIL_VIEW_DESC& dsvDesc) const noexcept
		{
			std::size_t seed = 0;
			dx_12_hash_combine(seed, dsvDesc.Format);
			dx_12_hash_combine(seed, dsvDesc.ViewDimension);
			dx_12_hash_combine(seed, dsvDesc.Flags);

			switch (dsvDesc.ViewDimension)
			{
			case D3D12_DSV_DIMENSION_TEXTURE1D:
				dx_12_hash_combine(seed, dsvDesc.Texture1D.MipSlice);
				break;
			case D3D12_DSV_DIMENSION_TEXTURE1DARRAY:
				dx_12_hash_combine(seed, dsvDesc.Texture1DArray.ArraySize);
				dx_12_hash_combine(seed, dsvDesc.Texture1DArray.FirstArraySlice);
				dx_12_hash_combine(seed, dsvDesc.Texture1DArray.MipSlice);
				break;
			case D3D12_DSV_DIMENSION_TEXTURE2D:
				dx_12_hash_combine(seed, dsvDesc.Texture2D.MipSlice);
				break;
			case D3D12_DSV_DIMENSION_TEXTURE2DARRAY:
				dx_12_hash_combine(seed, dsvDesc.Texture2DArray.ArraySize);
				dx_12_hash_combine(seed, dsvDesc.Texture2DArray.FirstArraySlice);
				dx_12_hash_combine(seed, dsvDesc.Texture2DArray.MipSlice);
				break;
			case D3D12_DSV_DIMENSION_TEXTURE2DMS:
				dx_12_hash_combine(seed, dsvDesc.Texture2DMS.UnusedField_NothingToDefine);
				break;
			case D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY:
				dx_12_hash_combine(seed, dsvDesc.Texture2DMSArray.ArraySize);
				dx_12_hash_combine(seed, dsvDesc.Texture2DMSArray.FirstArraySlice);
				break;

			}
			return seed;
		}
	};
	
	template<>
	struct hash<D3D12_SHADER_RESOURCE_VIEW_DESC>
	{
		std::size_t operator()(const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc) const noexcept
		{
			std::size_t seed = 0;

			dx_12_hash_combine(seed, srvDesc.Format);
			dx_12_hash_combine(seed, srvDesc.ViewDimension);
			dx_12_hash_combine(seed, srvDesc.Shader4ComponentMapping);

			switch (srvDesc.ViewDimension)
			{
			case D3D12_SRV_DIMENSION_BUFFER:
				dx_12_hash_combine(seed, srvDesc.Buffer.FirstElement);
				dx_12_hash_combine(seed, srvDesc.Buffer.NumElements);
				dx_12_hash_combine(seed, srvDesc.Buffer.StructureByteStride);
				dx_12_hash_combine(seed, srvDesc.Buffer.Flags);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE1D:
				dx_12_hash_combine(seed, srvDesc.Texture1D.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.Texture1D.MipLevels);
				dx_12_hash_combine(seed, srvDesc.Texture1D.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
				dx_12_hash_combine(seed, srvDesc.Texture1DArray.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.Texture1DArray.MipLevels);
				dx_12_hash_combine(seed, srvDesc.Texture1DArray.FirstArraySlice);
				dx_12_hash_combine(seed, srvDesc.Texture1DArray.ArraySize);
				dx_12_hash_combine(seed, srvDesc.Texture1DArray.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2D:
				dx_12_hash_combine(seed, srvDesc.Texture2D.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.Texture2D.MipLevels);
				dx_12_hash_combine(seed, srvDesc.Texture2D.PlaneSlice);
				dx_12_hash_combine(seed, srvDesc.Texture2D.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
				dx_12_hash_combine(seed, srvDesc.Texture2DArray.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.Texture2DArray.MipLevels);
				dx_12_hash_combine(seed, srvDesc.Texture2DArray.FirstArraySlice);
				dx_12_hash_combine(seed, srvDesc.Texture2DArray.ArraySize);
				dx_12_hash_combine(seed, srvDesc.Texture2DArray.PlaneSlice);
				dx_12_hash_combine(seed, srvDesc.Texture2DArray.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2DMS:
				dx_12_hash_combine(seed, srvDesc.Texture2DMS.UnusedField_NothingToDefine);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
				dx_12_hash_combine(seed, srvDesc.Texture2DMSArray.FirstArraySlice);
				dx_12_hash_combine(seed, srvDesc.Texture2DMSArray.ArraySize);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE3D:
				dx_12_hash_combine(seed, srvDesc.Texture3D.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.Texture3D.MipLevels);
				dx_12_hash_combine(seed, srvDesc.Texture3D.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_TEXTURECUBE:
				dx_12_hash_combine(seed, srvDesc.TextureCube.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.TextureCube.MipLevels);
				dx_12_hash_combine(seed, srvDesc.TextureCube.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
				dx_12_hash_combine(seed, srvDesc.TextureCubeArray.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.TextureCubeArray.MipLevels);
				dx_12_hash_combine(seed, srvDesc.TextureCubeArray.First2DArrayFace);
				dx_12_hash_combine(seed, srvDesc.TextureCubeArray.NumCubes);
				dx_12_hash_combine(seed, srvDesc.TextureCubeArray.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE:
				dx_12_hash_combine(seed, srvDesc.RaytracingAccelerationStructure.Location);
				break;
			}

			return seed;
		}
	};

	template<>
	struct hash<D3D12_UNORDERED_ACCESS_VIEW_DESC>
	{
		std::size_t operator()(const D3D12_UNORDERED_ACCESS_VIEW_DESC& uavDesc) const noexcept
		{
			std::size_t seed = 0;

			dx_12_hash_combine(seed, uavDesc.Format);
			dx_12_hash_combine(seed, uavDesc.ViewDimension);

			switch (uavDesc.ViewDimension)
			{
			case D3D12_UAV_DIMENSION_BUFFER:
				dx_12_hash_combine(seed, uavDesc.Buffer.FirstElement);
				dx_12_hash_combine(seed, uavDesc.Buffer.NumElements);
				dx_12_hash_combine(seed, uavDesc.Buffer.StructureByteStride);
				dx_12_hash_combine(seed, uavDesc.Buffer.CounterOffsetInBytes);
				dx_12_hash_combine(seed, uavDesc.Buffer.Flags);
				break;
			case D3D12_UAV_DIMENSION_TEXTURE1D:
				dx_12_hash_combine(seed, uavDesc.Texture1D.MipSlice);
				break;
			case D3D12_UAV_DIMENSION_TEXTURE1DARRAY:
				dx_12_hash_combine(seed, uavDesc.Texture1DArray.MipSlice);
				dx_12_hash_combine(seed, uavDesc.Texture1DArray.FirstArraySlice);
				dx_12_hash_combine(seed, uavDesc.Texture1DArray.ArraySize);
				break;
			case D3D12_UAV_DIMENSION_TEXTURE2D:
				dx_12_hash_combine(seed, uavDesc.Texture2D.MipSlice);
				dx_12_hash_combine(seed, uavDesc.Texture2D.PlaneSlice);
				break;
			case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
				dx_12_hash_combine(seed, uavDesc.Texture2DArray.MipSlice);
				dx_12_hash_combine(seed, uavDesc.Texture2DArray.FirstArraySlice);
				dx_12_hash_combine(seed, uavDesc.Texture2DArray.ArraySize);
				dx_12_hash_combine(seed, uavDesc.Texture2DArray.PlaneSlice);
				break;
			case D3D12_UAV_DIMENSION_TEXTURE3D:
				dx_12_hash_combine(seed, uavDesc.Texture3D.MipSlice);
				dx_12_hash_combine(seed, uavDesc.Texture3D.FirstWSlice);
				dx_12_hash_combine(seed, uavDesc.Texture3D.WSize);
				break;
			}

			return seed;
		}




	};

#ifdef _M_X64
#define ENABLE_SSE_CRC32 1
#else
#define ENABLE_SSE_CRC32 0
#endif

#if ENABLE_SSE_CRC32
#pragma intrinsic(_mm_crc32_u32)
#pragma intrinsic(_mm_crc32_u64)
#endif
	inline size_t HashRange(const uint32_t* const Begin, const uint32_t* const End, size_t Hash)
	{
#if ENABLE_SSE_CRC32
		const uint64_t* Iter64 = (const uint64_t*)(((uint64_t)Begin + 7) & ~7);
		const uint64_t* const End64 = (const uint64_t* const)((uint64_t)End & ~7);

		// If not 64-bit aligned, start with a single u32
		if ((uint32_t*)Iter64 > Begin)
			Hash = _mm_crc32_u32((uint32_t)Hash, *Begin);

		// Iterate over consecutive u64 values
		while (Iter64 < End64)
			Hash = _mm_crc32_u64((uint64_t)Hash, *Iter64++);

		// If there is a 32-bit remainder, accumulate that
		if ((uint32_t*)Iter64 < End)
			Hash = _mm_crc32_u32((uint32_t)Hash, *(uint32_t*)Iter64);
#else
		// An inexpensive hash for CPUs lacking SSE4.2
		for (const uint32_t* Iter = Begin; Iter < End; ++Iter)
			Hash = 16777619U * Hash ^ *Iter;
#endif

		return Hash;
	}

	template <typename T> 
	inline size_t HashState(const T* StateDesc, size_t Count = 1, size_t Hash = 2166136261U)
	{
		static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned");
		return HashRange((uint32_t*)StateDesc, (uint32_t*)(StateDesc + Count), Hash);
	}

}



