#pragma once
#include "DxObject.h"
#include "GUIAllocation.h"

namespace RE
{
	class Resource;
	class DescriptorAllocator;
	class DescriptorHandle;
	class CommandQueue;
	class CommandList;
	class Texture;
	class RenderTarget;
	class GUI;
	class Shader;
	// RenderTarget 제작
	// 간단한 물체 띄워보기
	// DynamicDescriptor 제작
	// Sample로 텍스쳐로 물체 띄워보기
	struct RENDERENGINE_API RenderDeviceDesc
	{
		HWND hWnd;
		DXGI_FORMAT format;
		uint32_t width;
		uint32_t height;
		uint32_t bufferCount;
	
		std::shared_ptr<GUI> gui;
		uint32_t gui_numDescriptor = 2048;
	};

	using RDFrameSubmission = std::function<void(CommandList*)>;
	class RENDERENGINE_API RenderDevice : public DxObject
	{
	public:
		RenderDevice(const RenderDeviceDesc& desc);
		~RenderDevice();
		
		// Frame Function
		void Update();


		/* Submit Function         쓰레드 0       쓰레드 1        쓰레드 2       쓰레드 3


 실행 : priority_number 0       command_id 0   command_id 1   command_id 2   command_id 3

 실행 : priority_number 1       command_id 0   command_id 1   command_id 2   command_id 3

 실행 : priority_number 2       command_id 0   command_id 1   command_id 2   command_id 3
		
		*/
		void SubmitToRender(uint32_t priority_number, const RDFrameSubmission& func);
		void SubmitToCompute(uint32_t priority_number,const RDFrameSubmission& func);
		void SubmitToCopy(uint32_t priority_number,    const RDFrameSubmission& func);
		void SubmitFinalTexture(const Texture& texture);

		// Allocate Function
		DescriptorHandle SrvAllocate(uint32_t numDescriptor = 1);
		DescriptorHandle UavAllocate(uint32_t numDescriptor = 1);
		DescriptorHandle CbvAllocate(uint32_t numDescriptor = 1);
		DescriptorHandle RtvAllocate(uint32_t numDescriptor = 1);
		DescriptorHandle DsvAllocate(uint32_t numDescriptor = 1);

		// GUI Allocate
		void GUIAllocate(const std::string& managedresourceName, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr);
		void GUIUnAllocate(const std::string& managedresourceName, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr);
		uint64_t GetGUIAllocation(const std::string& managedresourceName, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr);
		void RegisterGUITexture(const Texture& resource);
		const Resource& GetRegisteredGUITexture(const std::string& managedresourceName);
		// Etc...
		bool CompileShader(Shader& shader, const ShaderCompiler& compiler);
		void Resize(uint32_t width, uint32_t height);
	public:
		ID3D12Device* GetD3DDevice();
		IDXGIFactory4* GetDxgiFactory();
		uint64_t GetFrameCount() const;
		uint32_t GetValueIndex() const;
		const RenderDeviceDesc& GetDesc() const;
	private:
		void BeginFrame();
		void RenderUpdate();
		void ComputeUpdate();
		void CopyUpdate();
		void EndFrame();
		void Present(CommandList* commandList, const Texture& texture);
		void Flush();
	private:
		struct FrameResource
		{
			uint64_t DirectFenceValue;
			uint64_t ComputeFenceValue;
			uint64_t CopyFenceValue;
			uint64_t EngineFrameCount;

			FrameResource() :
				DirectFenceValue(0),
				ComputeFenceValue(0),
				CopyFenceValue(0),
				EngineFrameCount(0) { }
		};
	private:
		// DXGI or D3D 's Objects
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
		Microsoft::WRL::ComPtr<ID3D12Device>  m_Device;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;

		//  타입별 CommandQueue 및 프레임 리소스
		std::shared_ptr<CommandQueue> m_DirectCommandQueue;
		std::shared_ptr<CommandQueue> m_ComputeCommandQueue;
		std::shared_ptr<CommandQueue> m_CopyCommandQueue;
		std::vector<FrameResource> m_FrameResources;

		// 메모리 할당자
		std::shared_ptr<DescriptorAllocator>  m_SrvAllocator;
		std::shared_ptr<DescriptorAllocator>  m_UavAllocator;
		std::shared_ptr<DescriptorAllocator>  m_CbvAllocator;
		std::shared_ptr<DescriptorAllocator>  m_RtvAllocator;
		std::shared_ptr<DescriptorAllocator>  m_DsvAllocator;


		// 장치 프레임 업데이트 커맨드 리스트 관리자
		using RDFrameSubmissionPool = std::map<uint32_t, std::vector<RDFrameSubmission>>;
		RDFrameSubmissionPool m_RenderFrameSubmissionPool;
		RDFrameSubmissionPool m_ComputeFrameSubmissionPool;
		RDFrameSubmissionPool m_CopyFrameSubmissionPool;

		// 장치 변수 및 정보
		RenderDeviceDesc m_RenderDeviceDesc;
		uint64_t m_Frame;
		uint32_t m_ValueIndex;

		// 장치 리소스
		std::vector<std::shared_ptr<Texture>> m_BackBuffer;
		std::shared_ptr<Texture> m_FinalTexture;

		// GUI 할당
		std::unordered_map<size_t, GUIAllocation>  m_GUIAllocationMap;
		std::queue<GUIAllocation> m_PendingGUIAllocations;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_GUIDescriptorHeap;
		uint32_t m_GUIHeapOffset;
		uint32_t m_GUIHeapIncrementSize;
		uint32_t m_GUIMaxNumDescriptor;
		std::mutex m_GuiAllocationMutex;

		// GUI용 리소스 맵
		using ManagedTextureMap = std::unordered_map<std::string, Resource>;
		ManagedTextureMap m_ManagedTextureMap;
	};



}